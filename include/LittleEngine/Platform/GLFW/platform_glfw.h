#pragma once

#include "LittleEngine/Platform/GLFW/glfw_window.h"

namespace LittleEngine::Platform
{
    class PlatformGlfw {
    public:
        static bool Initialize();

        static void Shutdown();

        static bool IsInitialized();

        static std::unique_ptr<Window> MakeWindow(const WindowConfig& config);

    private:
        static bool s_initialized;
    };




} // namespace LittleEngine::Platform