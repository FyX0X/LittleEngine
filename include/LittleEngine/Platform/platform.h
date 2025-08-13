#pragma once

#include "LittleEngine/Core/window.h"

#ifdef USE_GLFW
#include "LittleEngine/Platform/GLFW/platform_glfw.h"
using PlatformImpl = LittleEngine::Platform::PlatformGlfw;
#elif defined(USE_SDL)
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