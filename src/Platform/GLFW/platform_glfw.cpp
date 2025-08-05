#include "LittleEngine/Platform/GLFW/platform_glfw.h"



#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace LittleEngine::Platform
{
    bool PlatformGlfw::Initialize()
    {
        if (!s_initialized) {
            s_initialized = (glfwInit() == GLFW_TRUE);
        }
        return s_initialized;
    }

    void PlatformGlfw::Shutdown()
    {
        if (s_initialized) {
            glfwTerminate();
            s_initialized = false;
        }
	}

    bool PlatformGlfw::IsInitialized()
    {
        return s_initialized;
    }
    std::unique_ptr<Window> PlatformGlfw::MakeWindow(const WindowConfig& config)
    {
        auto window = std::make_unique<GlfwWindow>();
        if (window->Initialize(config)) {
            return window;
        }
        return nullptr;
    }
	bool PlatformGlfw::s_initialized = false; // static member initialization


}