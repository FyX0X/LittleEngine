#pragma once

#include "LittleEngine/Core/window.h"

#if USE_GLFW == 1
#include "LittleEngine/Platform/GLFW/platform_glfw.h"
using PlatformImpl = LittleEngine::Platform::PlatformGlfw;
#elif USE_SDL == 1
#error "SDL platform is not implemented yet."
// not defined yet
#endif


namespace LittleEngine::Platform
{
	class Platform
	{
	public:
		static bool Initialize()
		{
			return PlatformImpl::Initialize();
		}
		static void Shutdown()
		{
			PlatformImpl::Shutdown();
		}
		static bool IsInitialized()
		{
			return PlatformImpl::IsInitialized();
		}

		static std::unique_ptr<Window> MakeWindow(const WindowConfig& config)
		{
			return PlatformImpl::MakeWindow(config);
		}
	};

} 