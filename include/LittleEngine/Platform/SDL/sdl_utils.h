#pragma once

#include "LittleEngine/Input/input.h"



namespace LittleEngine::Platform::SdlUtils
{


	Input::KeyCode KeyFromSdl(uint32_t key);

	Input::MouseButton MouseButtonFromSdl(uint8_t button);


}