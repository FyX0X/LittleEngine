#include "LittleEngine/Platform/GLFW/glfw_platform.h"


#include "LittleEngine/Utils/logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#if ENABLE_IMGUI == 1
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#endif // ENABLE_IMGUI


namespace LittleEngine::Platform
{

    bool GlfwPlatform::s_initialized = false; // static member initialization

    bool GlfwPlatform::Initialize()
    {
        if (!s_initialized) {
            s_initialized = (glfwInit() == GLFW_TRUE);
            if (!s_initialized) {
                Utils::Logger::Critical("Failed to initialize GLFW: " + std::to_string(glfwGetError(nullptr)));
                return false;
			}
            Utils::Logger::Info("GLFW Platform initialized successfully.");
        }
        return s_initialized;
    }

    void GlfwPlatform::Shutdown()
    {
        if (s_initialized) {
            glfwTerminate();
            s_initialized = false;
        }
	}

    bool GlfwPlatform::IsInitialized()
    {
        return s_initialized;
    }
    std::unique_ptr<Window> GlfwPlatform::MakeWindow(const WindowConfig& config)
    {
        auto window = std::make_unique<GlfwWindow>();
        if (window->Initialize(config)) {
            return window;
        }
        return nullptr;
    }


#if ENABLE_IMGUI == 1

    void GlfwPlatform::ImGuiInitialize(Window* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->GetNativeWindowHandle()), false);
        ImGui_ImplOpenGL3_Init("#version 330 core");
	}

    void GlfwPlatform::ImGuiShutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GlfwPlatform::ImGuiNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
	}    
    
    void GlfwPlatform::ImGuiRender()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

#endif


}