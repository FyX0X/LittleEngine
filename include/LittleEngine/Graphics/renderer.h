#pragma once
#include "LittleEngine/Graphics/camera.h"
#include "LittleEngine/Graphics/shader.h"
#include "LittleEngine/Graphics/color.h"
#include "LittleEngine/Graphics/texture.h"
#include "LittleEngine/Graphics/font.h"
#include "LittleEngine/Graphics/render_target.h"
#include "LittleEngine/Math/geometry.h"
#include <vector>
#include <array>
#include <unordered_map>


namespace LittleEngine::Graphics
{
	// Represents a rectangle {x, y, w, h} where x, y is the bottom left position.
	using Rect = glm::vec4;


	struct Vertex {
		glm::vec2 pos;
		glm::vec2 uv;
		Color color;
		float textureIndex;
	};


	class Renderer
	{
	public:

		Renderer() {};
		~Renderer() { Shutdown(); }

		Renderer(Renderer& other) = delete;
		Renderer(Renderer&& other) = delete;
		Renderer operator=(Renderer other) = delete;
		Renderer operator=(Renderer& other) = delete;
		Renderer operator=(Renderer&& other) = delete;

		void Initialize(const Camera& camera, glm::ivec2 size, unsigned int quadCount = defaults::QuadCount);
		void Shutdown();
		void UpdateWindowSize(glm::ivec2 size) 
		{
			UpdateWindowSize(size.x, size.y);
		}
		void UpdateWindowSize(int w, int h);

		/**
		 * Sets the current RenderTarget
		 * 
		 * @param: target: The render target, (defaults to the screen if = nullptr) 
		 */
		void SetRenderTarget(RenderTarget* target = nullptr);
		RenderTarget* GetRenderTarget() { return m_renderTarget; }
		void BeginFrame();
		void EndFrame();


		void SetWireframe(bool b);

		// flushes a fullscreen quad to the current render target
		// uses the current shader set by the user (shader.Use())
		void FlushFullscreenQuad();

		void BlitImage(const Texture& texture);

		void MergeLightScene(const Texture& scene, const Texture& light);


		void SaveScreenshot(RenderTarget* target = nullptr, const std::string& name = "screenshot");

#pragma region DRAW RECT

		// Render functions
		void DrawRect(const Rect& rect, const Sprite& sprite, const Color& color = Colors::White)
		{
			DrawRect(rect, sprite.texture, color, sprite.uvs);
		}
		void DrawRect(const Rect& rect, const Color& color = Colors::White)
		{
			Color colors[4] = { color, color, color, color };
			DrawRect(rect, s_defaultTexture, colors, {0, 0, 1, 1});
		}
		void DrawRect(const Rect& rect, Texture texture, const Color& color = Colors::White)
		{
			Color colors[4] = { color, color, color, color };
			DrawRect(rect, texture, colors, { 0, 0, 1, 1 });
		}
		void DrawRect(const Rect& rect, Texture texture, const Color& color, const glm::vec4& uv)
		{
			Color colors[4] = { color, color, color, color };
			DrawRect(rect, texture, colors, uv);
		}
		void DrawRect(const Rect& rect, Texture texture, const Color colors[4], const glm::vec4& uv);

#pragma endregion

#pragma region DRAW LINE

		void DrawLine(const glm::vec2& p1, const glm::vec2& p2, float width = 1.f, Color color = Colors::White)
		{
			DrawLine({ p1, p2 }, width, color);
		}
		void DrawLine(const Math::Edge& e, float width = 1.f, Color color = Colors::White);

#pragma endregion

#pragma region DRAW POLYGON
		// Only works with convex polygons
		void DrawPolygon(const Math::Polygon& polygon, const Color& color = Colors::White);
		void DrawPolygonOutline(const Math::Polygon& polygon, float width = 1.f, const Color& color = Colors::White);

#pragma endregion

#pragma region DRAW TEXT

		void DrawString(const std::string& text, const glm::vec2 pos, Color color = Colors::White, float scale = 1.f) 
		{
			DrawString(text, pos, s_defaultFont, color, scale);
		}
		void DrawString(const std::string& text, const glm::vec2 pos, const Font& font, Color color = Colors::White, float scale = 1.f);

#pragma endregion


		// Clears the current render target with the given color
		void Clear(const Color& color = Colors::ClearColor);

		int GetQuadCount() { return m_quadCount; }

		enum class BlendMode {
			None,
			Alpha,
			Additive,
			Multiply
		};

		void SetBlendMode(BlendMode mode);


		//Camera camera = {};
		Shader shader;

		void SetCamera(const Camera& camera) { m_camera = &camera; }
		const Camera& GetCamera() const { return *m_camera; }

		void Flush();

		void BindScreen();
	private:
		// TODO REFACTOR AUTO FLUSH IF FULL NOT AT END
		// FLUSH BATCH IF: > 16 textures || #indices > 65536 (or > 10K quads)
		void RenderBatch();
		int AddTextureToBatch(Texture texture);
		void ClearDrawQueue();
		void ClearBatch();

		bool m_isInitialized = false;


		static Texture s_defaultTexture;
		static Font s_defaultFont;


		const Camera* m_camera = nullptr;

		int m_width = -1;
		int m_height = -1;

		int m_quadCount = 0;

		GLuint m_VAO = 0;
		GLuint m_VBO = 0;
		GLuint m_EBO = 0;

		RenderTarget* m_renderTarget = nullptr;

		std::vector<Vertex> m_vertices;
		std::vector<Vertex> m_verticesBatch;
		std::vector<unsigned int> m_indices;
		std::vector<unsigned int> m_indicesBatch;
		std::vector<Texture> m_textures;
		std::array<Texture, defaults::MAX_TEXTURE_SLOTS> m_texturesBatch;
		int m_bindedTextureCount = 0;


		// uniform texture sampler
		const int m_samplers[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };


		unsigned int m_fullscreenVAO = 0;
		unsigned int m_fullscreenVBO = 0;

		Shader m_blitShader = {};
		Shader m_mergeLightSceneShader = {};

	};

}