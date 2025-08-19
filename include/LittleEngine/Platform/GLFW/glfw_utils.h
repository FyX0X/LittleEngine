#pragma once

#include "LittleEngine/Input/input.h"

namespace LittleEngine::Platform::GlfwUtils
{


	Input::KeyCode KeyFromGlfw(int key);

	Input::MouseButton MouseButtonFromGlfw(int button);


}