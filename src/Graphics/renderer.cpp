#include "LittleEngine/Graphics/renderer.h"
#include "LittleEngine/Graphics/bitmap_helper.h"

#include "LittleEngine/Utils/logger.h"
#include "LittleEngine/internal.h"
#include "LittleEngine/Utils/file_system.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image/stb_image_write.h>


#include <sstream>
#include <filesystem>


namespace LittleEngine::Graphics
{

	const std::string fullQuadVertexShader = R"(
		#version 330 core
		layout(location = 0) in vec2 aPos;
		layout(location = 1) in vec2 aTexCoord;

		out vec2 TexCoords;

		void main()
		{
			TexCoords = aTexCoord;
			gl_Position = vec4(aPos, 0.0, 1.0);
		}
    )";

	const std::string blitImageFragmentShader = R"(
		#version 330 core

		out vec4 FragColor;
		in vec2 TexCoords; // Texture coordinates for the fragment

		uniform sampler2D uTexture; // Texture to sample from

		void main()
		{
			FragColor = texture(uTexture, TexCoords); // Sample the texture at the given coordinates
		}
    )";

	const std::string mergeFragmentShader = R"(
		#version 330 core
		in vec2 TexCoords;
		out vec4 FragColor;

		uniform sampler2D sceneTexture;
		uniform sampler2D lightTexture;

		void main()
		{
			vec3 sceneColor = texture(sceneTexture, TexCoords).rgb;
			vec3 lightColor = texture(lightTexture, TexCoords).rgb;

			// Additive blending of light on scene
			vec3 finalColor = sceneColor * lightColor;

			// Optional: tone mapping or clamp
			finalColor = clamp(finalColor, 0.0, 1.0);

			FragColor = vec4(finalColor, 1.0);
		}
    )";


	Texture Renderer::s_defaultTexture = Texture();
	Font Renderer::s_defaultFont = Font();

#pragma region init / shut down

	void Renderer::Initialize(const Camera& camera, glm::ivec2 size, unsigned int quadCount)
	{

		if (!internal::g_initialized)
			Utils::Logger::Critical("RENDERER::INIT : library not initialized.");

		if (m_isInitialized)
		{
			Utils::Logger::Warning("RENDERER::INIT : Renderer Already initized.");
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
			Utils::Logger::Error("OpenGL Error during Renderer::Initialize - Code: " + std::to_string(err));
		}
#endif


		UpdateWindowSize(size);


		m_blitShader.Create(fullQuadVertexShader, blitImageFragmentShader, false);
		m_blitShader.Use();
		m_blitShader.SetInt("uTexture", 0); // set texture sampler to 0

		m_mergeLightSceneShader.Create(fullQuadVertexShader, mergeFragmentShader, false);
		m_mergeLightSceneShader.Use();
		m_mergeLightSceneShader.SetInt("sceneTexture", 0); // set scene texture sampler to 0
		m_mergeLightSceneShader.SetInt("lightTexture", 1); // set light texture sampler to 1

		shader.CreateDefault();
		shader.Use();		// important to use shader at end of initialization.

		SetBlendMode(BlendMode::Alpha);

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
		{
			Utils::Logger::Warning("RENDERER::DrawRect : texture not loaded.");
			texture = s_defaultTexture;	// use default texture
		}

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

	void Renderer::DrawLine(const Math::Edge& e, float width, Color color)
	{

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
	
	void Renderer::DrawPolygon(const Math::Polygon& polygon, const Color& color)
	{

		// check if polygon is valid
		if (!polygon.IsValid())
			return;

		int vertexCount = static_cast<int>(polygon.vertices.size());


		int triangleCount = vertexCount - 2; // number of triangles in the polygon

		// if triangle count is even => good
		// if triangle count is odd => make a false quad by duplicating the last vertex

		Math::Polygon poly = polygon; // copy polygon to modify it

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

	void Renderer::DrawPolygonOutline(const Math::Polygon& polygon, float width, const Color& color)
	{
		// check if polygon is valid
		if (!polygon.IsValid())
			return;

		for (Math::Edge& e : polygon.GetEdges())
		{
			DrawLine(e, width, color);
		}
	}

	void Renderer::DrawString(const std::string& text, const glm::vec2 pos, const Font& font, Color color, float scale)
	{
		if (font.GetTexture().id == 0)
			return;

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

	void Renderer::Clear(const Color& color)
	{
		if (!m_isInitialized)
		{
			Utils::Logger::Warning("RENDERER::CLEAR : library was not initialized.");
			return;
		}

		if (m_width <= 0 || m_height <= 0)
		{
			Utils::Logger::Warning("Renderer::Clear: framebuffer size <= 0 : skipping clear.");
			return;
		}

		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}

#pragma endregion

#pragma region Frame

	void Renderer::SetRenderTarget(RenderTarget* target)
	{
		if (!m_indices.empty())
		{
			Flush();	// flush to previous render Target.
		}

		if (m_renderTarget == target)	// already binded.
			return;

		m_renderTarget = target;
		int width, height;
		if (target)
		{
			target->Bind(); // bind the new target
			width = target->GetSize().x;
			height = target->GetSize().y;
		}
		else
		{
			BindScreen(); // bind the default framebuffer (screen)
			width = m_width;
			height = m_height;
		}
		glViewport(0, 0, width, height); // reset viewport to target size
	}

	void Renderer::BeginFrame()
	{
		Clear(); // clear the current render target
		
		
		ClearDrawQueue();

	}

	void Renderer::EndFrame()
	{
		Flush(); // render everything queued
	}

	void Renderer::SetWireframe(bool b)
	{
		if (b)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void Renderer::SaveScreenshot(RenderTarget* target, const std::string& name)
	{
		RenderTarget* old = GetRenderTarget();

		int width, height;
		int imageTarget;

		if (target == nullptr)	// window
		{
			width = m_width;
			height = m_height;
			SetRenderTarget();
			glReadBuffer(GL_BACK);
		}
		else 
		{
			width = target->GetSize().x;
			height = target->GetSize().y;
			SetRenderTarget(target);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
		}

		std::vector<unsigned char> pixels(width * height * 4);

		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());


		// swap pixels because opengl has bottom left as origin, instead of top left.
		FlipBitmapVertically(pixels.data(), width, height, 4);

		Utils::FileSystem::CreateDirectories("screenshots");

		std::string path = Utils::FileSystem::GetNextFreeFilepath("screenshots", name, ".png");


		stbi_write_png(path.c_str(), width, height, 4, pixels.data(), width * 4);

		SetRenderTarget(old);
		
	}
	
	void Renderer::SetBlendMode(BlendMode mode)
	{
		switch (mode)
		{
			case BlendMode::None:
				glDisable(GL_BLEND);
				break;
			case BlendMode::Alpha:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquation(GL_FUNC_ADD);
				break;
			case BlendMode::Additive:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE); // Additive blending
				glBlendEquation(GL_FUNC_ADD);
				break;
			case BlendMode::Multiply:
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_COLOR, GL_ZERO); // Multiply blending
				glBlendEquation(GL_FUNC_ADD);
				break;
		}
	}

	void Renderer::Flush()
	{
		if (!internal::g_initialized)
		{
			Utils::Logger::Error("RENDERER::FLUSH : library was not initialized.");
			return;
		}

		if (m_width < 0 || m_height < 0)
		{
			Utils::Logger::Error("RENDERER::FLUSH : window size was negative.");
			return;
		}

		if (!m_VAO)
		{
			Utils::Logger::Error("RENDERER::FLUSH : renderer not initialized.");
			return;
		}

		if (m_vertices.empty() || m_indices.empty())
		{
			//LogWarning("Renderer::Flush : Nothing to draw!");
			return;
		}

		if (m_textures.empty())
		{
			Utils::Logger::Warning("Renderer::Flush : texture buffer empty.");
			return;
		}

		if (m_camera == nullptr)
		{
			Utils::Logger::Warning("Renderer::Flush : Camera not set.");
			return;
		}


		if (m_width == 0 || m_height == 0)
		{
			Utils::Logger::Warning("Renderer::Flush: size is zero : (" + std::to_string(m_width) + ", " +std::to_string(m_height) + "), skipping flush.");
			ClearDrawQueue();
			return;
		}


		// Bind shader
		shader.Use();

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
				{
					Utils::Logger::Error("RENDERER::FLUSH : texture slot full even after flush.");
					return;
				}

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
			Utils::Logger::Error("OpenGL Error in Renderer::RenderBatch() - Code: " + std::to_string(err));
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

	void Renderer::BlitImage(const Texture& texture)
	{
		if (texture.id == 0)
			return;
		
		m_blitShader.Use(); // Use the blit shader
		//m_blitShader.SetInt("uTexture", 0); // Set the texture sampler to 0 NO need to set it again, already set in Initialize
		texture.Bind(0); // Bind texture to slot 0
		
		FlushFullscreenQuad(); // Render the fullscreen quad with the bound texture

		texture.Unbind(0); // Unbind the texture after rendering

		shader.Use(); // Reset to default shader after blitting
	}

	void Renderer::MergeLightScene(const Texture& scene, const Texture& light)
	{
		if (scene.id == 0 || light.id == 0)
			return;

		m_mergeLightSceneShader.Use(); // Use the merge shader
		scene.Bind(0); // Bind scene texture to slot 0
		light.Bind(1); // Bind light texture to slot 1
		FlushFullscreenQuad(); // Render the fullscreen quad with the bound textures

		shader.Use(); // Reset to default shader after merging
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



