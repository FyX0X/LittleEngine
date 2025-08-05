#pragma once


#include <LittleEngine/Math/geometry.h>
#include <LittleEngine/Graphics/shader.h>
#include <LittleEngine/Graphics/render_target.h>
#include <LittleEngine/Graphics/camera.h>
#include <LittleEngine/Graphics/color.h>
#include <LittleEngine/Graphics/renderer.h>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace LittleEngine::Graphics
{

	struct ShadowQuad
	{
		glm::vec2 p1, p2, p3, p4;
	};

	// Light source struct
	struct LightSource
	{
		glm::vec2 position = { 0.f, 0.f };
		glm::vec3 color = { 1.f, 1.f, 1.f };
		float intensity = 1.f;
		float radius = 1.f;


		std::vector<ShadowQuad> GetShadowQuads(const Math::Polygon& poly) const;
	};


	// Returns a vector containing the vertices of the shadow triangles ready for rendering.
	std::vector<glm::vec2> GetShadowTriangles(const std::vector<ShadowQuad>& shadowQuads);

	class LightSystem
	{
	public:
		LightSystem() {};
		~LightSystem() { Shutdown(); }

		LightSystem(LightSystem& other) = delete;
		LightSystem(LightSystem&& other) = delete;
		LightSystem operator=(LightSystem other) = delete;
		LightSystem operator=(LightSystem& other) = delete;
		LightSystem operator=(LightSystem&& other) = delete;

		void Initialize(size_t maxQuadCount = 1000);
		void Shutdown();

#pragma region Light and shadow rendering

		void RenderLighting(Renderer* renderer, RenderTarget* target, bool enableShadows = true, const Color& color = Colors::Black);
		void RenderPrecomputedLighting(Renderer* renderer, RenderTarget* target, bool enableShadows = true, const Color& color = Colors::Black);

#pragma endregion


		// Updates the lights and obstacles in the scene.
#pragma region LightSource management

		LightSource* CreateLightSource(glm::vec2 pos, glm::vec3 color, float intensity, float radius);

		// Deletes a light source from the scene.
		// Returns true if the light source was found and deleted, false otherwise.
		// IMPORTANT: the lightSource pointer becomes invalid after deletion, do not use it anymore!
		bool DeleteLightSource(LightSource* lightSource);

		/**
		 * Deletes all light sources in the scene.
		 * IMPORTANT: after this call, all light source pointers become invalid, do not use them anymore!
		 */
		void ClearLightSources() { m_lightSources.clear(); }
		const std::vector<std::unique_ptr<LightSource>>& GetLightSources() const { return m_lightSources; }
#pragma endregion

#pragma region Obstacle management

		Math::Polygon* CreateObstacle(std::vector<glm::vec2> vertices);

		// Deletes an obstacle from the scene.
		// Returns true if the obstacle was found and deleted, false otherwise.
		// IMPORTANT: the Math::Polygon pointer becomes invalid after deletion, do not use it anymore!
		bool DeleteObstacle(Math::Polygon* polygon);

		/**
		 * Deletes all obstacles in the scene.
		 * IMPORTANT: after this call, all obstacles pointers become invalid, do not use them anymore!
		 */
		void ClearObstacles() { m_obstacles.clear(); }
		const std::vector<std::unique_ptr<Math::Polygon>>& GetObstacles() const { return m_obstacles; }

		void PrecomputeShadowVertices();


#pragma endregion

	private:

		void BatchShadows()
		{
			BatchVertices(m_shadowVertices);
			m_shadowVertices.clear(); // clear the vertices after batching
		}
		void BatchVertices(const std::vector<glm::vec2>& vertices);

		bool m_initialized = false; // true if the light system is initialized

		size_t m_maxQuadCount = -1;

		// unique_ptrs to own the light sources and obstacles
		std::vector<std::unique_ptr<LightSource>> m_lightSources; // List of light sources in the scene
		std::vector<std::unique_ptr<Math::Polygon>> m_obstacles; // List of obstacles in the scene

		// light and shadows rendering

		GLuint shadowVAO = 0;
		GLuint shadowVBO = 0;

		Shader m_shadowShader = {};
		Shader m_lightShader = {};

		RenderTarget m_tempLightFBO = {};

		std::vector<glm::vec2> m_shadowVertices; // vertices for shadow rendering


		// precompute all shadow vertices for each light source and obstacle
		std::vector<std::vector<std::vector<glm::vec2>>> m_verticesLightBatches; // shadow vertices for each light source

	};

}