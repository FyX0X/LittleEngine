
#include "LittleEngine/Graphics/renderer.h"
#include "LittleEngine/Graphics/bitmap_helper.h"

#include "LittleEngine/error_logger.h"
#include "LittleEngine/internal.h"
#include "LittleEngine/Storage/storage_helper.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image/stb_image_write.h>


#include <sstream>
#include <filesystem>


namespace LittleEngine::Graphics
{

	Texture Renderer::s_defaultTexture = Texture();
	Font Renderer::s_defaultFont = Font();

#pragma region init / shut down

	void Renderer::Initialize(const Camera& camera, glm::ivec2 size, unsigned int quadCount)
	{

		if (!internal::g_initialized)
			ThrowError("RENDERER::INIT : library not initialized.");

		if (m_isInitialized)
		{
			LogError("RENDERER::INIT : Renderer Already initized.");
			return;	
		}
		m_isInitialized = true;

		SetCamera(camera);

		m_vertices.reserve(quadCount * 4);
		m_indices.reserve(quadCount * 6);
		m_textures.reserve(quadCount);

		// initialize buffer objects
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		//glBufferData(GL_ARRAY_BUFFER, quadCount * 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadCount * 6 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);


		// position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(1);
		// color attribute
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(2);
		// texture index attribute
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureIndex));
		glEnableVertexAttribArray(3);

		glBindVertexArray(0);

		// set default texture / font
		s_defaultTexture = Texture::GetDefaultTexture();
		s_defaultFont = Font::GetDefaultFont(16.f);

		// TODO CHANGE DEFAULT PATH

#ifdef _DEBUG
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			LogError("OpenGL Error during Renderer::Initialize - Code: " + std::to_string(err));
		}
#endif


		UpdateWindowSize(size);

		shader.CreateDefault();
		shader.Use();

	}

	void Renderer::Shutdown()
	{
		if (!m_isInitialized)
			return;


		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);

		m_VAO = 0;
		m_VBO = 0;
		m_EBO = 0;

		m_vertices.clear();
		m_indices.clear();
		m_textures.clear();
		m_isInitialized = false;
	}


#pragma endregion

#pragma region Callback

	void Renderer::UpdateWindowSize(int w, int h)
	{
		m_width = w;
		m_height = h;
	}

#pragma endregion

#pragma region Draw functions

	void Renderer::DrawRect(const Rect& rect, Texture texture, const Color colors[4], const glm::vec4& uv)
	{

		if (texture.id == 0)	// problem
			ThrowError("RENDERER::DrawRect : texture not loaded.");

		// rect {x, y, w, h} (x,y) is bottom left
		// uv is usually (0, 0, 1, 1) (u_min, v_min, u_max, v_max)

		// position
		glm::vec2 p0{ rect.x, rect.y };						// bottom left
		glm::vec2 p1{ rect.x + rect.z, rect.y };			// bottom right
		glm::vec2 p2{ rect.x + rect.z, rect.y + rect.w };	// top right
		glm::vec2 p3{ rect.x, rect.y + rect.w };			// top left


		// UVs
		glm::vec2 uv0{ uv.x, uv.y };		// bottom left
		glm::vec2 uv1{ uv.z, uv.y };		// bottom right
		glm::vec2 uv2{ uv.z, uv.w };		// top right
		glm::vec2 uv3{ uv.x, uv.w };		// top left

		// TODO ADD ROTATION LATER
		// MAYBE 

		Vertex v0 = { p0, uv0, colors[0], texture.id};
		Vertex v1 = { p1, uv1, colors[1], texture.id};
		Vertex v2 = { p2, uv2, colors[2], texture.id};
		Vertex v3 = { p3, uv3, colors[3], texture.id};


		int index = m_vertices.size();

		m_vertices.push_back(v0);
		m_vertices.push_back(v1);
		m_vertices.push_back(v2);
		m_vertices.push_back(v3);

		m_indices.push_back(index + 0);
		m_indices.push_back(index + 1);
		m_indices.push_back(index + 2);
		m_indices.push_back(index + 0);
		m_indices.push_back(index + 2);
		m_indices.push_back(index + 3);

		m_textures.push_back(texture);

		m_quadCount++;
	}

	void Renderer::DrawLine(const Edge& e, float width, Color color)
	{
		if (s_defaultTexture.id == 0)	// problem
			ThrowError("RENDERER::DrawLine : default texture not loaded.");

		glm::vec2 halfWidthVector = e.normal() * width / 2.f;	// right normal


		// position
		glm::vec2 p0 = e.p1 + halfWidthVector;
		glm::vec2 p1 = e.p2 + halfWidthVector;
		glm::vec2 p2 = e.p2 - halfWidthVector;
		glm::vec2 p3 = e.p1 - halfWidthVector;


		// UVs
		glm::vec2 uv0{ 0, 0 };		// bottom left
		glm::vec2 uv1{ 1, 0 };		// bottom right
		glm::vec2 uv2{ 1, 1 };		// top right
		glm::vec2 uv3{ 0, 1 };		// top left


		Vertex v0 = { p0, uv0, color, s_defaultTexture.id };
		Vertex v1 = { p1, uv1, color, s_defaultTexture.id };
		Vertex v2 = { p2, uv2, color, s_defaultTexture.id };
		Vertex v3 = { p3, uv3, color, s_defaultTexture.id };


		int index = m_vertices.size();

		m_vertices.push_back(v0);
		m_vertices.push_back(v1);
		m_vertices.push_back(v2);
		m_vertices.push_back(v3);

		m_indices.push_back(index + 0);
		m_indices.push_back(index + 1);
		m_indices.push_back(index + 2);
		m_indices.push_back(index + 0);
		m_indices.push_back(index + 2);
		m_indices.push_back(index + 3);

		m_textures.push_back(s_defaultTexture);

		m_quadCount++;

	}


	
	void Renderer::DrawPolygon(const Polygon& polygon, const Color& color)
	{
		if (s_defaultTexture.id == 0)	// problem
			ThrowError("RENDERER::DrawLine : default texture not loaded.");

		// check if polygon is valid
		if (!polygon.IsValid())
		{
			LogError("Renderer::DrawPolygon : Polygon is not valid.");
			return;
		}

		int vertexCount = static_cast<int>(polygon.vertices.size());

		//int index = m_vertices.size();

		//// push all vertices to the vertex buffer
		//for (size_t i = 0; i < vertexCount; i++)
		//{
		//	Vertex v{ polygon.vertices[i], {0, 0}, color, s_defaultTexture.id };
		//	m_vertices.push_back(v);
		//}


		//for (int i = 1; i < vertexCount - 1; ++i)
		//{
		//	//draw a triangle from the first vertex to the current vertex and the next vertex
		//	m_indices.push_back(index + 0); // first vertex
		//	m_indices.push_back(index + i); // current vertex
		//	m_indices.push_back(index + i + 1); // next vertex
		//}

		//// add the default texture to the texture buffer
		//m_textures.push_back(s_defaultTexture);

		//// update the quad count ???????????
		//m_quadCount += vertexCount - 2;		// overcounting, but not a problem for now



		// NOT working because flush uses quads only.

		int triangleCount = vertexCount - 2; // number of triangles in the polygon

		// if triangle count is even => good
		// if triangle count is odd => make a false quad by duplicating the last vertex

		Polygon poly = polygon; // copy polygon to modify it

		if (triangleCount % 2 != 0)
		{
			poly.vertices.push_back(polygon.vertices.back()); // duplicate last vertex
			vertexCount++;
			triangleCount++;
		}

		// all quads are (0123, 0345, 0567, ...)
		for (size_t i = 0; i < triangleCount / 2; i++)
		{
			int index = m_vertices.size();
			Vertex v0{ poly.vertices[0], {0, 0}, color, s_defaultTexture.id }; // first vertex
			Vertex v1{ poly.vertices[2 * i + 1], {0, 0}, color, s_defaultTexture.id }; // current vertex
			Vertex v2{ poly.vertices[2 * i + 2], {0, 0}, color, s_defaultTexture.id }; // next vertex
			Vertex v3{ poly.vertices[2 * i + 3], {0, 0}, color, s_defaultTexture.id }; // first vertex again for the quad
			m_vertices.push_back(v0);
			m_vertices.push_back(v1);
			m_vertices.push_back(v2);
			m_vertices.push_back(v3);

			m_indices.push_back(index + 0);
			m_indices.push_back(index + 1);
			m_indices.push_back(index + 2);
			m_indices.push_back(index + 0);
			m_indices.push_back(index + 2);
			m_indices.push_back(index + 3);

			m_textures.push_back(s_defaultTexture);

			m_quadCount++;
		}


	}

	void Renderer::DrawPolygonOutline(const Polygon& polygon, float width, const Color& color)
	{
		if (s_defaultTexture.id == 0)	// problem
			ThrowError("RENDERER::DrawPolygonOutline : default texture not loaded.");

		// check if polygon is valid
		if (!polygon.IsValid())
		{
			LogError("Renderer::DrawPolygonOutline : Polygon is not valid.");
			return;
		}

		for (Edge& e : polygon.GetEdges())
		{
			DrawLine(e, width, color);
		}
	}


	void Renderer::DrawString(const std::string& text, const glm::vec2 pos, const Font& font, Color color, float scale)
	{
		if (font.GetTexture().id == 0)
		{
			LogError("Renderer::DrawString : Font uninitialized.");
			return;
		}

		font.Bind();

		// position of the cursor.
		float xpos = pos.x;
		float ypos = pos.y - scale;


		float factor = scale / font.GetSize();

		for (char c : text)
		{
			if (c == '\n') {
				// New line: reset x, move y down by line height
				xpos = pos.x;
				ypos -= scale;
				continue;
			}

			const GlyphInfo* g = font.GetGlyph(c);
			
			if (g == nullptr)
				continue;
						
			
			Rect rect{
				xpos + g->bearing.x * factor, ypos - (g->size.y - g->bearing.y) * factor,
				g->size.x * factor, g->size.y * factor
			};

			DrawRect(rect, font.GetTexture(), color, g->uv);

			xpos += g->advance * factor;

		}

		font.Unbind();
	}

#pragma endregion

#pragma region Frame

	void Renderer::SetRenderTarget(RenderTarget* target)
	{
		if (!m_indices.empty())
		{
			Flush();	// flush to previous render Target.
		}

		m_renderTarget = target;
		if (target)
		{
			target->Bind(); // bind the new target
		}
		else
		{
			BindScreen(); // bind the default framebuffer (screen)
		}
		int width, height;
		if (target == nullptr)	// window
		{
			width = m_width;
			height = m_height;
		}
		else 
		{
			width = target->GetSize().x;
			height = target->GetSize().y;
		}
		glViewport(0, 0, width, height); // reset viewport to target size
	}

	void Renderer::BeginFrame()
	{
		glClearColor(Colors::ClearColor.r, Colors::ClearColor.g, Colors::ClearColor.b, Colors::ClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT);
		
		
		ClearDrawQueue();

	}

	void Renderer::EndFrame()
	{
		Flush(); // render everything queued
	}


	void Renderer::SaveScreenshot(RenderTarget* target, const std::string& name)
	{
		int width, height;
		int imageTarget;

		if (target == nullptr)	// window
		{
			width = m_width;
			height = m_height;
			glReadBuffer(GL_BACK);
		}
		else 
		{
			width = target->GetSize().x;
			height = target->GetSize().y;
			target->Bind();
			glReadBuffer(GL_COLOR_ATTACHMENT0);
		}

		std::vector<unsigned char> pixels(width * height * 4);

		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

		if (target != nullptr)
			target->Unbind();

		// swap pixels because opengl has bottom left as origin, instead of top left.
		FlipBitmapVertically(pixels.data(), width, height, 4);

		Storage::EnsureDirectoryExists("screenshots");

		std::string path = Storage::GetNextFreeFilepath("screenshots", "screenshot" + name, ".png");


		stbi_write_png(path.c_str(), width, height, 4, pixels.data(), width * 4);


		
	}
	
	void Renderer::Flush()
	{
		if (!internal::g_initialized)
			ThrowError("RENDERER::FLUSH : library was not initialized.");

		if (m_width < 0 || m_height < 0)
			ThrowError("RENDERER::FLUSH : window size was negative.");

		if (!m_VAO)
			ThrowError("RENDERER::FLUSH : renderer not initialized.");

		if (m_vertices.empty() || m_indices.empty())
		{
			LogWarning("Renderer::Flush : Nothing to draw!");
			return;
		}

		if (m_textures.empty())
		{
			LogWarning("Renderer::Flush : texture buffer empty.");
			return;
		}

		if (m_camera == nullptr)
		{
			LogError("Renderer::Flush : Camera not set.");
			return;
		}

		if (m_renderTarget == nullptr)	// draw to screen
		{
			glViewport(0, 0, m_width, m_height);
		}
		//else
		//{
		//	glm::ivec2 size = m_renderTarget->GetSize();
		//	glViewport(0, 0, size.x, size.y);
		//	m_renderTarget->Bind();
		//}

		// Bind shader
		//shader.Use();

		// initializes uniform variables
		shader.SetMat4("view", m_camera->GetViewMatrix());
		shader.SetMat4("projection", m_camera->GetProjectionMatrix());
		// set uniform texture sampler
		// TODO: MAYBE MOVE SOMEWHERE ELSE IF NOT NEEDED EACH FRAME
		for (int i = 0; i < defaults::MAX_TEXTURE_SLOTS; ++i)
		{
			std::string uniformName = "uTex" + std::to_string(i);
			shader.SetInt(uniformName, i); // Bind uTex{i} to texture unit i
		}

		//shader.SetIntArray("uTextures", defaults::MAX_TEXTURE_SLOTS, m_samplers);

		size_t quadCountInBatch = 0;

		for (size_t i = 0; i < m_textures.size(); ++i)
		{
			int slot = AddTextureToBatch(m_textures[i]);
			if (slot == -1)
			{
				// Batch full, flush current batch
				RenderBatch();
				//ClearBatch();

				// Add current texture again (should succeed now)
				slot = AddTextureToBatch(m_textures[i]);
				if (slot == -1)
					ThrowError("RENDERER::FLUSH : texture slot full even after flush.");

				quadCountInBatch = 0;
			}

			// Add quad vertices with texture slot
			for (int v = 0; v < 4; ++v)
			{
				Vertex vert = m_vertices[i * 4 + v];
				vert.textureIndex = static_cast<float>(slot);
				m_verticesBatch.push_back(vert);
			}

			// Add quad indices adjusted relative to current batch
			unsigned int quadVertexStart = i * 4;
			unsigned int batchVertexStart = quadCountInBatch * 4;

			for (int idx = 0; idx < 6; ++idx)
			{
				unsigned int originalIndex = m_indices[i * 6 + idx];
				unsigned int localIndex = originalIndex - quadVertexStart;
				m_indicesBatch.push_back(localIndex + batchVertexStart);
			}

			++quadCountInBatch;
		}

		if (quadCountInBatch > 0)
		{
			RenderBatch();
		}


		//if (m_renderTarget != nullptr)
		//	m_renderTarget->Unbind();


		ClearDrawQueue();	// clear draw queue after flushing

	}

	void Renderer::RenderBatch()
	{

		// binding vertex array
		glBindVertexArray(m_VAO);

		// upload vertex data
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_verticesBatch.size() * sizeof(Vertex), m_verticesBatch.data(), GL_STREAM_DRAW);

		// Upload index data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesBatch.size() * sizeof(unsigned int), m_indicesBatch.data(), GL_STREAM_DRAW);


		// draw data
		glDrawElements(GL_TRIANGLES, m_indicesBatch.size(), GL_UNSIGNED_INT, nullptr);


		// Optionally unbind VAO (not strictly needed)
		glBindVertexArray(0);

		ClearBatch();

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			LogError("OpenGL Error in Renderer::RenderBatch() - Code: " + std::to_string(err));
		}

	}

	int Renderer::AddTextureToBatch(Texture texture)
	{
		// check if texture already bound
		GLuint new_id = texture.id;
		for (int slot = 0; slot < m_bindedTextureCount; slot++)
		{
			if (m_texturesBatch[slot].id == new_id)
				return slot;
		}

		// if binded texture full, render batch
		if (m_bindedTextureCount == defaults::MAX_TEXTURE_SLOTS)
			return -1;

		// bind the texture.
		m_texturesBatch[m_bindedTextureCount] = texture;
		texture.Bind(m_bindedTextureCount);

		return m_bindedTextureCount++;

	}


	void Renderer::FlushFullscreenQuad()
	{
		if (m_fullscreenVAO == 0) {
			// First-time setup
			float quadVertices[] = {
				// positions   // tex coords
				-1.0f,  1.0f,  0.0f, 1.0f,  // top-left
				-1.0f, -1.0f,  0.0f, 0.0f,  // bottom-left
				 1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right

				-1.0f,  1.0f,  0.0f, 1.0f,  // top-left
				 1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right
				 1.0f,  1.0f,  1.0f, 1.0f   // top-right
			};

			glGenVertexArrays(1, &m_fullscreenVAO);
			glGenBuffers(1, &m_fullscreenVBO);

			glBindVertexArray(m_fullscreenVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_fullscreenVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

			// Position (vec2)
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			// TexCoord (vec2)
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		// Draw
		glBindVertexArray(m_fullscreenVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

#pragma endregion

#pragma region Clear frame / batch

	void Renderer::ClearDrawQueue()
	{
		// clear vectors
		m_vertices.clear();
		m_indices.clear();
		m_textures.clear();

		m_quadCount = 0;
	}

	void Renderer::ClearBatch()
	{
		m_verticesBatch.clear();
		m_indicesBatch.clear();
		m_texturesBatch.fill(Texture{});
		m_bindedTextureCount = 0;
	}

#pragma endregion


	void Renderer::BindScreen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_width, m_height); // reset viewport to target size
	}

#pragma region Getters


#pragma endregion
}



