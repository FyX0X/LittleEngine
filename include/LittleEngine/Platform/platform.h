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

		inline bool Initialize() { return PlatformImpl::Initialize(); }
		inline void Shutdown() { PlatformImpl::Shutdown(); }
		inline bool IsInitialized() { return PlatformImpl::IsInitialized(); }
		inline std::unique_ptr<Window> MakeWindow(const WindowConfig& config) { return PlatformImpl::MakeWindow(config); }

#if ENABLE_IMGUI == 1
		inline void ImGuiInitialize(Window* window) { PlatformImpl::ImGuiInitialize(window); }
		inline void ImGuiShutdown() { PlatformImpl::ImGuiShutdown(); }
		inline void ImGuiNewFrame() { PlatformImpl::ImGuiNewFrame(); }
		inline void ImGuiRender() { PlatformImpl::ImGuiRender(); }
#endif



} 