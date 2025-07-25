#pragma once
#include <glm/glm.hpp>


namespace LittleEngine::Graphics
{

	using Color = glm::vec4;
	namespace Colors
	{
		inline const Color Red { 1, 0, 0, 1 };
		inline const Color Green { 0, 1, 0, 1 };
		inline const Color Blue { 0, 0, 1, 1 };
		inline const Color Black { 0, 0, 0, 1 };
		inline const Color White { 1, 1, 1, 1 };
		inline const Color Yellow { 1, 1, 0, 1 };
		inline const Color Magenta { 1, 0, 1, 1 };
		inline const Color Turqoise { 0, 1, 1, 1 };
		inline const Color Orange { 1, (float)0x7F / 255.0f, 0, 1 };
		inline const Color Purple { 101.0f / 255.0f, 29.0f / 255.0f, 173.0f / 255.0f, 1 };
		inline const Color Gray { (float)0x7F / 255.0f, (float)0x7F / 255.0f, (float)0x7F / 255.0f, 1 };
		inline const Color Transparent { 0,0,0,0 };
		inline const Color ClearColor{ 0.2f, 0.3f, 0.3f, 1.0f };

	}
}