#pragma once


namespace LittleEngine::Graphics
{

	// Light source struct
	struct LightSource
	{
		glm::vec2 position = { 0.f, 0.f };
		glm::vec3 color = { 1.f, 1.f, 1.f };
		float intensity = 1.f;
		float radius = 1.f;
	};



}