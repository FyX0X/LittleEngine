#pragma once

#include "LittleEngine/Platform/SDL/sdl_window.h"



namespace LittleEngine::Platform
{
    class SdlPlatform {
    public:
        static bool Initialize();

        static void Shutdown();

        static bool IsInitialized();

        static std::unique_ptr<Window> MakeWindow(const WindowConfig& config);

#if ENABLE_IMGUI == 1

        static void ImGuiInitialize(Window* window);
        static void ImGuiShutdown();
		static void ImGuiNewFrame();
        static void ImGuiRender();

#endif

    private:
        static bool s_initialized;
    };




} // namespace LittleEngine::Platform