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

	void Renderer::Initialize(glm::ivec2 size, unsigned int quadCount)
	{

		if (!internal::g_initialized)
			ThrowError("RENDERER::INIT : library not initialized.");

		if (m_isInitialized)
		{
			LogError("RENDERER::INIT : Renderer Already initized.");
			return;	
		}
		m_isInitialized = true;

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
	}

	void Renderer::BeginFrame()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		
		ClearDrawQueue();

	}

	void Renderer::EndFrame()
	{
		Flush(); // render everything queued
	}


	void Renderer::SaveScreenshot(RenderTarget* target)
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

		std::string path = Storage::GetNextFreeFilepath("screenshots", "screenshot", ".png");


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

		if (m_renderTarget == nullptr)	// draw to screen
		{
			glViewport(0, 0, m_width, m_height);
		}
		else
		{
			glm::ivec2 size = m_renderTarget->GetSize();
			glViewport(0, 0, size.x, size.y);
			m_renderTarget->Bind();
		}

		// Bind shader
		shader.Use();

		// initializes uniform variables
		shader.SetMat4("view", camera.GetViewMatrix());
		shader.SetMat4("projection", GetProjectionMatrix());
		// set uniform texture sampler
		// TODO: MAYBE MOVE SOMEWHERE ELSE IF NOT NEEDED EACH FRAME
		shader.SetIntArray("uTextures", defaults::MAX_TEXTURE_SLOTS, m_samplers);

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


		if (m_renderTarget != nullptr)
			m_renderTarget->Unbind();


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

#pragma region Getters

	glm::mat4 Renderer::GetProjectionMatrix()
	{
		float aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);
		float halfWidth, halfHeight;
		if (aspectRatio >= 16.f/9.f)
		{
			halfWidth = defaults::viewWidth / 2;
			halfHeight = halfWidth / aspectRatio;
		}
		else
		{
			halfHeight = defaults::viewHeight / 2;
			halfWidth = halfHeight * aspectRatio;

		}
		// tODO change this
		return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.f, 1.f);
	}

#pragma endregion
}



