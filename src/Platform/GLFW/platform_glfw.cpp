#include "LittleEngine/Platform/GLFW/platform_glfw.h"



#include <glad/glad.h>
#include <GLFW/glfw3.h>


#if ENABLE_IMGUI == 1
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#endif // ENABLE_IMGUI


namespace LittleEngine::Platform
{

    bool PlatformGlfw::s_initialized = false; // static member initialization

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


#if ENABLE_IMGUI == 1

    void PlatformGlfw::ImGuiInitialize(Window* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->GetNativeWindowHandle()), true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
	}

    void PlatformGlfw::ImGuiShutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void PlatformGlfw::ImGuiNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
	}    
    
    void PlatformGlfw::ImGuiRender()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

#endif


}