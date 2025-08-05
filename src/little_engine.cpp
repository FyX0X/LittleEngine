//////////////////////////
// LittleEngine library	//
// author: Martin Ergo	//
//////////////////////////

#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>







#include "LittleEngine/error_logger.h"
#include "LittleEngine/little_engine.h"


#include "LittleEngine/Platform/platform.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <chrono>



// remove
#include <thread>



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
	static WindowState s_windowState = {};
	//static ResizeCallback s_windowResizeCallback = nullptr;
	static const float s_updateTimeStep = 1.f / 60.f; // update step in seconds (60 FPS)
	static float accumulatedTime = 0.f; // accumulated time for update steps
	static float s_fps = 0.f; // frames per second

#pragma region function pre definition;
	namespace
	{
		//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		//void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		//void processInput(GLFWwindow* window);
		//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		void PrintStackTrace();
		void APIENTRY glDebugOutput(GLenum source,
			GLenum type,
			unsigned int id,
			GLenum severity,
			GLsizei length,
			const char* message,
			const void* userParam);
	}

#pragma endregion

#pragma region Library Management

	int Initialize(const EngineConfig& config)
	{
		if (internal::g_initialized)
		{
			LogError("LittleEngine has already been initialized.");
			return 1;
		}
		internal::g_initialized = true;


		// TODO: move this to a better place, like a platform specific file or debug utility.
#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
		// Initialize call stack recording
		SymInitialize(GetCurrentProcess(), NULL, TRUE);
#endif

		// INITIALIZE GLFW
#pragma region PLATFORM INIT


		Platform::Platform::Initialize();




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
		s_window = Platform::Platform::MakeWindow(windowConfig);



		// check if debug was initialized correctly.
		int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

#pragma endregion



#if ENABLE_IMGUI == 1
		Platform::Platform::ImGuiInitialize(s_window.get());
#endif


		// create default shader
		Graphics::Shader::Initialize();
		Graphics::Font::Initialize();
		Input::Initialize(static_cast<GLFWwindow*>(s_window->GetNativeWindowHandle()), LittleEngine::GetWindowSize());

		return 0;

	}


	void Shutdown()
	{

#if ENABLE_IMGUI == 1
		Platform::Platform::ImGuiShutdown();
#endif

		// platform: terminate, clearing all previously allocated resources.
		// ------------------------------------------------------------------
		Platform::Platform::Shutdown();
	}


#pragma endregion

#pragma region MainLoop

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


#if ENABLE_IMGUI == 1
			Platform::Platform::ImGuiNewFrame();
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


#if ENABLE_IMGUI == 1
			Platform::Platform::ImGuiRender();
#endif

			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			s_window->OnUpdate();
		}


		std::chrono::duration<float> totalTime = lastTime - firstTime;
		std::cout << "Total average fps: " << (float)frameCount / totalTime.count();


	}

#pragma endregion

#pragma region Getters

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

#pragma region Setters

	void SetWindowResizeCallback(ResizeCallback callback)
	{
		s_window->SetWindowResizeCallback(callback);
	}



#pragma endregion

#pragma region Render options


	void SetVsync(bool b)
	{
		s_window->SetVsync(b);
	}



#pragma endregion

#pragma region Internal

	// internal only functions
	namespace 
	{




		// Custom callbackFunction to pass to openGL for debugging
		void APIENTRY glDebugOutput(GLenum source,
			GLenum type,
			unsigned int id,
			GLenum severity,
			GLsizei length,
			const char* message,
			const void* userParam)
		{
			// ignore non-significant error/warning codes
			if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

			std::cout << "---------------" << std::endl;
			std::cout << "Debug message (" << id << "): " << message << std::endl;

			switch (source)
			{
			case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
			case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
			case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
			} std::cout << std::endl;

			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
			case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
			case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
			case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
			case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
			} std::cout << std::endl;

			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
			case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
			case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
			} std::cout << std::endl;
			std::cout << std::endl;

			PrintStackTrace();

		}


	}

#pragma endregion

}
