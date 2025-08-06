#include "LittleEngine/Graphics/lighting.h"

#include "LittleEngine/Utils/logger.h"


namespace LittleEngine::Graphics
{


	const std::string shadowVertexShader = R"(
		#version 330 core
		layout(location = 0) in vec2 aPos;

		uniform mat4 proj;
		uniform mat4 view;

		void main()
		{
			gl_Position = proj * view * vec4(aPos, 0.0, 1.0);
		}
    )";

	const std::string shadowFragmentShader = R"(
		#version 330 core
		out vec4 FragColor;


		void main()
		{	
			// Set shadow color black.
			FragColor = vec4(0, 0, 0, 1.0);
		}
    )";

	const std::string lightVertexShader = R"(
		#version 330 core
		layout(location = 0) in vec2 aPos;      // NDC
		layout(location = 1) in vec2 aTexCoord;


		void main()
		{
			gl_Position = vec4(aPos, 0.0, 1.0);
		}
	)";

	const std::string lightFragmentShader = R"(
		#version 330 core

		out vec4 FragColor;


		uniform mat4 uInvProj;
		uniform mat4 uInvView;

		uniform vec2 uLightPos;       // Light position in world coords
		uniform vec3 uLightColor;     // Light RGB color
		uniform float uLightRadius;   // Light radius (max influence)
		uniform float uLightIntensity; // Light intensity scalar

		uniform vec2 uScreenSize;      // Needed to convert gl_FragCoord to NDC

		void main()
		{
			// Get NDC coords from screen coords
			vec2 ndc = (gl_FragCoord.xy / uScreenSize) * 2.0 - 1.0;
			vec4 clipSpace = vec4(ndc, 0.0, 1.0);

			// Convert to world space
			vec4 worldSpace = uInvView * uInvProj * clipSpace;
			vec2 worldPos = worldSpace.xy / worldSpace.w;


			// Calculate distance from current fragment to light center
			float dist = length(worldPos - uLightPos);

			// If outside light radius, no contribution
			if (dist > uLightRadius)
				discard;

			// Compute attenuation (inverse quadratic falloff)
			float attenuation = 1.0 - (dist / uLightRadius);
			attenuation = attenuation * attenuation; // quadratic falloff

			// Final light color with intensity and attenuation
			vec3 light = uLightColor * attenuation * uLightIntensity;

			FragColor = vec4(light, 1.0);
		}
	)";


	std::vector<ShadowQuad> LightSource::GetShadowQuads(const Math::Polygon& poly) const
	{
		std::vector<ShadowQuad> shadowQuads;
		for (const Math::Edge& edge : poly.GetEdges())
		{
			if (Math::ThreePointOrientation(edge.p1, edge.p2, position) == 1)	// if clockwise, position in on the right side of the edge (faces light)
			{
				glm::vec2 dir1 = glm::normalize(edge.p1 - position);
				glm::vec2 dir2 = glm::normalize(edge.p2 - position);
				// Create shadow quad vertices
				glm::vec2 p1 = edge.p1;
				glm::vec2 p2 = edge.p2;
				glm::vec2 p3 = edge.p2 + dir2 * radius * 100.f;
				glm::vec2 p4 = edge.p1 + dir1 * radius * 100.f;
				shadowQuads.push_back({ p1, p2, p3, p4 });
			}
		}

		return shadowQuads;
	}

	std::vector<glm::vec2> GetShadowTriangles(const std::vector<ShadowQuad>& shadowQuads)
	{
		std::vector<glm::vec2> triangles;
		for (const ShadowQuad& quad : shadowQuads)
		{
			// Create two triangles from the quad
			triangles.push_back(quad.p1);
			triangles.push_back(quad.p2);
			triangles.push_back(quad.p3);
			triangles.push_back(quad.p1);
			triangles.push_back(quad.p3);
			triangles.push_back(quad.p4);
		}
		return triangles;
	}

#pragma region Light System

	void LightSystem::Initialize(size_t maxQuadCount)
	{
		if (!internal::g_initialized)
		{
			Utils::Logger::Warning("LightSystem::Initialize : library not initialized.");
			return;
		}

		if (m_initialized)
		{
			Utils::Logger::Warning("LightSystem::Initialize : LightSystem already initialized.");
			return;
		}

		m_initialized = true;
		m_maxQuadCount = maxQuadCount;

		// initialize buffers
		m_shadowVertices.reserve(maxQuadCount * 6);		// reserve space for maxQuadCount shadow quads (6 vertices each)
		m_shadowVertices.clear();

		glGenVertexArrays(1, &shadowVAO);
		glGenBuffers(1, &shadowVBO);
		glBindVertexArray(shadowVAO);
		glBindBuffer(GL_ARRAY_BUFFER, shadowVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 6 * m_maxQuadCount, nullptr, GL_DYNAMIC_DRAW);		// alocates space for 6 vertices (2 triangles) * m_maxQuadCount
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);		// position attribute
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		// initialize shaders
		m_lightShader.Create(lightVertexShader, lightFragmentShader, false);
		m_shadowShader.Create(shadowVertexShader, shadowFragmentShader, false);

		// initialize temporary light FBO
		m_tempLightFBO.Create(1, 1, GL_RGB16F);

	}

	void LightSystem::Shutdown()
	{
		if (!m_initialized)
		{
			Utils::Logger::Warning("LightSystem::Shutdown : LightSystem not initialized.");
			return;
		}
		m_lightSources.clear();
		m_obstacles.clear();
		//m_lightShader.Cleanup();
		//m_shadowShader.Cleanup();

		m_tempLightFBO.Cleanup();

		glDeleteVertexArrays(1, &shadowVAO);
		glDeleteBuffers(1, &shadowVBO);

		shadowVAO = 0;
		shadowVBO = 0;

		m_initialized = false;

	}

	void LightSystem::RenderLighting(Renderer* renderer, RenderTarget* target, bool enableShadows, const Color& color)
	{
		if (!m_initialized)
		{
			Utils::Logger::Warning("LightSystem::RenderLighting : LightSystem not initialized.");
			return;
		}

		if (!renderer)
		{
			Utils::Logger::Warning("LightSystem::RenderLighting : Renderer is null.");
			return;
		}

		if (!target)
		{
			Utils::Logger::Warning("LightSystem::RenderLighting : RenderTarget is null.");
			return;
		}

		// store previous renderTarget to restore it afterward.
		RenderTarget* old = renderer->GetRenderTarget();

		// recreate the temp FBO for correct size.
		if (m_tempLightFBO.GetSize() != target->GetSize())	
		{
			m_tempLightFBO.Cleanup(); // cleanup previous FBO if it exists
			glm::ivec2 size = target->GetSize();
			m_tempLightFBO.Create(size.x, size.y, GL_RGB16F);
		}


		renderer->SetRenderTarget(target);

		// clear target and set background lighting color
		renderer->Clear(color);

		const Camera& camera = renderer->GetCamera();

		for (const auto& lightSource : m_lightSources)
		{
			if (!lightSource) continue; // skip null light sources

			renderer->SetBlendMode(Renderer::BlendMode::None);

			renderer->SetRenderTarget(&m_tempLightFBO);
			renderer->Clear(Colors::Black); // clear the temporary light FBO

			// Set up light shader
			m_lightShader.Use();
			m_lightShader.SetMat4("uInvProj", glm::inverse(camera.GetProjectionMatrix()));
			m_lightShader.SetMat4("uInvView", glm::inverse(camera.GetViewMatrix()));
			m_lightShader.SetVec2("uScreenSize", target->GetSize());
			m_lightShader.SetVec2("uLightPos", lightSource->position);
			m_lightShader.SetVec3("uLightColor", lightSource->color);
			m_lightShader.SetFloat("uLightRadius", lightSource->radius);
			m_lightShader.SetFloat("uLightIntensity", lightSource->intensity);
			// Draw the light volume
			renderer->FlushFullscreenQuad();


			if (enableShadows)
			{
				// Draw shadows
				m_shadowShader.Use();
				m_shadowShader.SetMat4("proj", camera.GetProjectionMatrix());
				m_shadowShader.SetMat4("view", camera.GetViewMatrix());

				m_shadowVertices.clear(); // clear previous shadow vertices
				for (const auto& obstacle : m_obstacles)
				{
					if (!obstacle) continue; // skip null obstacles
					std::vector<ShadowQuad> shadowQuads = lightSource->GetShadowQuads(*obstacle);
					std::vector<glm::vec2> vertices = GetShadowTriangles(shadowQuads);

					// add new vertices
					if (m_shadowVertices.size() + vertices.size() > m_maxQuadCount * 6)
						BatchShadows(); // flush previous batch if it exceeds max capacity

					m_shadowVertices.insert(m_shadowVertices.end(), vertices.begin(), vertices.end());

				}

				BatchShadows(); // draw all accumulated shadows
			}

			// Add the temporary light FBO to the main target
			renderer->SetBlendMode(Renderer::BlendMode::Additive);
			renderer->SetRenderTarget(target);

			renderer->BlitImage(m_tempLightFBO.GetTexture());

		}

		renderer->SetBlendMode(Renderer::BlendMode::Alpha); // reset blend mode to default
		renderer->SetRenderTarget(old); // reset to previous target
		renderer->shader.Use();

	}

	void LightSystem::RenderPrecomputedLighting(Renderer* renderer, RenderTarget* target, bool enableShadows, const Color& color)
	{
		if (!m_initialized)
		{
			Utils::Logger::Warning("LightSystem::RenderLighting : LightSystem not initialized.");
			return;
		}
		if (m_verticesLightBatches.size() != m_lightSources.size())
		{
			Utils::Logger::Warning("LightSystem::RenderLighting : Precomputed shadow vertices not initialized.");
			return;
		}

		if (!renderer)
		{
			Utils::Logger::Warning("LightSystem::RenderLighting : Renderer is null.");
			return;
		}

		if (!target)
		{
			Utils::Logger::Warning("LightSystem::RenderLighting : RenderTarget is null.");
			return;
		}

		// store previous renderTarget to restore it afterward.
		RenderTarget* old = renderer->GetRenderTarget();

		// recreate the temp FBO for correct size.
		if (m_tempLightFBO.GetSize() != target->GetSize())
		{
			m_tempLightFBO.Cleanup(); // cleanup previous FBO if it exists
			glm::ivec2 size = target->GetSize();
			m_tempLightFBO.Create(size.x, size.y, GL_RGB16F);
		}


		renderer->SetRenderTarget(target);

		// clear target and set background lighting color
		renderer->Clear(color);

		const Camera& camera = renderer->GetCamera();

		for (size_t i = 0; i < m_lightSources.size(); i++)
		{
			const auto& lightSource = m_lightSources[i];

			if (!lightSource) continue; // skip null light sources

			renderer->SetBlendMode(Renderer::BlendMode::None);

			renderer->SetRenderTarget(&m_tempLightFBO);
			renderer->Clear(Colors::Black); // clear the temporary light FBO

			// Set up light shader
			m_lightShader.Use();
			m_lightShader.SetMat4("uInvProj", glm::inverse(camera.GetProjectionMatrix()));
			m_lightShader.SetMat4("uInvView", glm::inverse(camera.GetViewMatrix()));
			m_lightShader.SetVec2("uScreenSize", target->GetSize());
			m_lightShader.SetVec2("uLightPos", lightSource->position);
			m_lightShader.SetVec3("uLightColor", lightSource->color);
			m_lightShader.SetFloat("uLightRadius", lightSource->radius);
			m_lightShader.SetFloat("uLightIntensity", lightSource->intensity);
			// Draw the light volume
			renderer->FlushFullscreenQuad();


			if (enableShadows)
			{
				// Draw shadows
				m_shadowShader.Use();
				m_shadowShader.SetMat4("proj", camera.GetProjectionMatrix());
				m_shadowShader.SetMat4("view", camera.GetViewMatrix());

				// render all batches for this light source
				for (size_t j = 0; j < m_verticesLightBatches[i].size(); j++)
				{
					const auto& batch = m_verticesLightBatches[i][j];


					BatchVertices(batch); // flush previous batch if it exceeds max capacity


				}

			}

			// Add the temporary light FBO to the main target
			renderer->SetBlendMode(Renderer::BlendMode::Additive);
			renderer->SetRenderTarget(target);

			renderer->BlitImage(m_tempLightFBO.GetTexture());

		}

		renderer->SetBlendMode(Renderer::BlendMode::Alpha); // reset blend mode to default
		renderer->SetRenderTarget(old); // reset to previous target
		renderer->shader.Use();

	}

	void LightSystem::PrecomputeShadowVertices()
	{
		if (!m_initialized)
		{
			Utils::Logger::Warning("LightSystem::PrecomputeShadowVertices : LightSystem not initialized.");
			return;
		}
		m_verticesLightBatches.clear(); // clear previous batches
		for (const auto& lightSource : m_lightSources)
		{
			std::vector<std::vector<glm::vec2>> batches;
			std::vector<glm::vec2> batch;
			if (!lightSource) continue; // skip null light sources
			for (const auto& obstacle : m_obstacles)
			{
				if (!obstacle) continue; // skip null obstacles
				std::vector<ShadowQuad> shadowQuads = lightSource->GetShadowQuads(*obstacle);
				std::vector<glm::vec2> vertices = GetShadowTriangles(shadowQuads);
				// add new vertices
				if (batch.size() + vertices.size() > m_maxQuadCount * 6)
				{
					// add current batch to the list and start a new one
					batches.push_back(batch);
					batch.clear();
				}
				batch.insert(batch.end(), vertices.begin(), vertices.end());
			}
			// add the last batch if not empty
			if (!batch.empty())
			{
				batches.push_back(batch);
				batch.clear();
			}

			// add the batches for this light source
			m_verticesLightBatches.push_back(batches);
		}
		
	}

	void LightSystem::BatchVertices(const std::vector<glm::vec2>& vertices)
	{
		glBindVertexArray(shadowVAO);
		glBindBuffer(GL_ARRAY_BUFFER, shadowVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * vertices.size(), vertices.data());
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}



	LightSource* LightSystem::CreateLightSource(glm::vec2 pos, glm::vec3 color, float intensity, float radius)
	{
		LightSource l = { pos, color, intensity, radius };
		m_lightSources.push_back(std::make_unique<LightSource>(l));
		return m_lightSources.back().get();
	}

	bool LightSystem::DeleteLightSource(LightSource* lightSource)
	{
		auto it = std::remove_if(m_lightSources.begin(), m_lightSources.end(),
			[lightSource](const std::unique_ptr<LightSource>& ls) { return ls.get() == lightSource; });
		if (it != m_lightSources.end())	// found
		{
			m_lightSources.erase(it, m_lightSources.end());
			return true;
		}
		return false;

		//LogError("LightSystem::DeleteLightSource : Light source not found.");
	}

	Math::Polygon* LightSystem::CreateObstacle(std::vector<glm::vec2> vertices)
	{
		Math::Polygon poly = { vertices };
		if (!poly.IsValid())
		{
			Utils::Logger::Warning("LightSystem::CreateObstacle : Polygon is not valid.");
			return nullptr;
		}

		m_obstacles.push_back(std::make_unique<Math::Polygon>(poly));
		return m_obstacles.back().get();
	}

	bool LightSystem::DeleteObstacle(Math::Polygon* polygon)
	{
		auto it = std::remove_if(m_obstacles.begin(), m_obstacles.end(),
			[polygon](const std::unique_ptr<Math::Polygon>& p) { return p.get() == polygon; });
		if (it != m_obstacles.end())	// found
		{
			m_obstacles.erase(it, m_obstacles.end());
			return true;
		}
		//  not found
		return false;
	
		//LogError("LightSystem::DeleteObstacle : Polygon not found.");
		
	}


#pragma endregion
}