//////////////////////////
// LittleEngine library	//
// author: Martin Ergo	//
//////////////////////////

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>







#include "LittleEngine/Utils/logger.h"
#include "LittleEngine/Utils/debug_tools.h"
#include "LittleEngine/little_engine.h"


#include "LittleEngine/Platform/platform.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <chrono>



// remove
#include <thread>


#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif


//if you are not using visual studio make shure you link to "Opengl32.lib"
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4244 4305 4267 4996 4018)
#pragma comment(lib, "Opengl32.lib")
#endif


using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;


namespace LittleEngine
{
	static std::unique_ptr<Window> s_window = {};
	static const float s_updateTimeStep = 1.f / 60.f; // update step in seconds (60 FPS)
	static float accumulatedTime = 0.f; // accumulated time for update steps
	static float s_fps = 0.f; // frames per second


#pragma region Library Management

	int Initialize(const EngineConfig& config)
	{
		if (internal::g_initialized)
		{
			Utils::Logger::Error("LittleEngine has already been initialized.");
			return 1;
		}
		internal::g_initialized = true;


		Utils::DebugTools::InitializeDebugTools();

		//Inialize the logger
		//Utils::Logger::SetLogFile("logs/LittleEngine.log");


		// INITIALIZE GLFW
#pragma region PLATFORM INIT


		Platform::Initialize();




#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


		WindowConfig windowConfig;
		windowConfig.title = config.title;
		windowConfig.width = config.windowWidth;
		windowConfig.height = config.windowHeight;
		windowConfig.mode = config.windowMode;
		windowConfig.maximized = config.maximized;
		windowConfig.vsyncEnabled = config.vsync;
		windowConfig.iconPath = config.iconPath;
		s_window = Platform::MakeWindow(windowConfig);

		// TODO : where to put this?

		// check if debug was initialized correctly.


#pragma endregion



#ifdef ENABLE_IMGUI
		Platform::ImGuiInitialize(s_window.get());
#endif


		// create default shader
		Graphics::Shader::Initialize();
		Graphics::Font::Initialize();
		Input::Initialize(static_cast<GLFWwindow*>(s_window->GetNativeWindowHandle()), LittleEngine::GetWindowSize());

		return 0;

	}


	void Shutdown()
	{

#ifdef ENABLE_IMGUI
		Platform::ImGuiShutdown();
#endif

		// platform: terminate, clearing all previously allocated resources.
		// ------------------------------------------------------------------
		Platform::Shutdown();
	}


#pragma endregion


	void Run(const std::function<void(float)>& update, const std::function<void()>& render)
	{


		TimePoint lastTime = Clock::now();
		TimePoint firstTime = lastTime;

		int frameCount = 0;


		// render loop
		// -----------
		while (!s_window->ShouldClose())
		{
			frameCount++;			// TODO REMOVE THIS WHEN NOT NEEDED

			// INIT
			// -----

			int w, h;
			s_window->GetWindowSize(w, h);

			// input
			// -----
			Input::UpdateInputState();


#ifdef ENABLE_IMGUI
			Platform::ImGuiNewFrame();
#endif


			// deltaTime
			// ------

			TimePoint currentTime = Clock::now();
			std::chrono::duration<float> delta = currentTime - lastTime;


			//// Sleep if we're early
			//const float targetFrameTime = 1.f / 5.f;
			//std::chrono::duration<float> remaining = std::chrono::duration<float>(targetFrameTime) - delta;
			//if (remaining.count() > 0.f)
			//{
			//	// Sleep most of it
			//	std::this_thread::sleep_for(remaining - std::chrono::milliseconds(1));

			//	// Busy-wait the rest (for precision)
			//	do {
			//		currentTime = Clock::now();
			//		delta = currentTime - lastTime;
			//	} while (delta.count() < targetFrameTime);
			//}


			lastTime = currentTime;

			// Update game at fixed time step.
			s_fps = 1.f / delta.count(); // update fps
			accumulatedTime += delta.count();
			while (accumulatedTime >= s_updateTimeStep)
			{
				accumulatedTime -= s_updateTimeStep;
				// Update the game state
				update(s_updateTimeStep);
			}

			render();


			// finalize input (for previous key state)
			Input::UpdatePreviousInputState();


#ifdef ENABLE_IMGUI
			Platform::ImGuiRender();
#endif

			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			s_window->OnUpdate();
		}


		std::chrono::duration<float> totalTime = lastTime - firstTime;
		std::cout << "Total average fps: " << (float)frameCount / totalTime.count();


	}

#pragma region Getters

	Window* LittleEngine::GetWindow()
	{
		return s_window.get();
	}


	glm::ivec2 GetWindowSize()
	{
		int w, h;
		s_window->GetWindowSize(w, h);
		return glm::ivec2(w, h);
	}

	float GetFPS()
	{
		return s_fps;
	}

#pragma endregion


	void SetVsync(bool b)
	{
		s_window->SetVsync(b);
	}



}
