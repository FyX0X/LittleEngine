#include "LittleEngine/Platform/SDL/sdl_platform.h"



#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "LittleEngine/Utils/logger.h"


#if ENABLE_IMGUI == 1
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>
#endif // ENABLE_IMGUI


namespace LittleEngine::Platform
{

    bool SdlPlatform::s_initialized = false; // static member initialization

    bool SdlPlatform::Initialize()
    {
        if (!s_initialized) {
            SDL_ClearError();
            s_initialized = SDL_Init(SDL_INIT_VIDEO);
            if (!s_initialized) {
                Utils::Logger::Critical(std::string("Failed to initialize SDL: ") + SDL_GetError());
                return false;
			}
			Utils::Logger::Info("SDL Platform initialized successfully.");
        }
        return s_initialized;
    }

    void SdlPlatform::Shutdown()
    {
        if (s_initialized) {
            SDL_Quit();
            s_initialized = false;
        }
	}

    bool SdlPlatform::IsInitialized()
    {
        return s_initialized;
    }

    std::unique_ptr<Window> SdlPlatform::MakeWindow(const WindowConfig& config)
    {
        auto window = std::make_unique<SdlWindow>();
        if (window->Initialize(config)) {
            return window;
        }
        return nullptr;
    }


#if ENABLE_IMGUI == 1

    void SdlPlatform::ImGuiInitialize(Window* window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // Setup Platform/Renderer backends
        // Initialize backends
        ImGui_ImplSDL3_InitForOpenGL(static_cast<SDL_Window*>(window->GetNativeWindowHandle()), window->GetNativeContext());
        ImGui_ImplOpenGL3_Init("#version 330 core");  // GLSL version
	}

    void SdlPlatform::ImGuiShutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void SdlPlatform::ImGuiNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
	}    
    
    void SdlPlatform::ImGuiRender()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

#endif


}