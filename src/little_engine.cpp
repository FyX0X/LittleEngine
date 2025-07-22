//////////////////////////
// LittleEngine library	//
// author: Martin Ergo	//
//////////////////////////

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if ENABLE_IMGUI == 1
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#endif // 

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <execinfo.h>
#include <unistd.h>
#include <cxxabi.h>
#include <cstdlib>
#endif



#include "LittleEngine/little_engine.h"
#include "LittleEngine/error_logger.h"

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
	static GLFWwindow* s_window = {};
	static ResizeCallback s_windowResizeCallback = nullptr;

#pragma region function pre definition;
	namespace
	{
		//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		//void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		//void processInput(GLFWwindow* window);
		void framebuffer_size_callback(GLFWwindow* window, int width, int height);
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


		// Initialize call stack recording
		SymInitialize(GetCurrentProcess(), NULL, TRUE);


		// INITIALIZE GLFW
#pragma region GLFW INIT


		// glfw: initialize and configure
		// ------------------------------
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// enable more debugging
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);


#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// glfw window creation
		// --------------------
		s_window = glfwCreateWindow(config.windowWidth, config.windowHeight, config.title.c_str(), NULL, NULL);
		if (s_window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(s_window);
		glfwSetFramebufferSizeCallback(s_window, framebuffer_size_callback);




		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}

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


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



#pragma region ImGui setup

#if ENABLE_IMGUI == 1
		// ImGui

// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Setup style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(s_window, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");


#endif
#pragma endregion


		// default vsync:
		SetVsync(config.vsync);

		// load icon
		if (!config.iconPath.empty())
		{
			SetApplicationIcon(config.iconPath);
		}

		// create default shader
		Graphics::Shader::Initialize();
		Graphics::Font::Initialize();
		Input::Initialize(s_window);

		return 0;

	}


	void Shutdown()
	{

#if ENABLE_IMGUI == 1
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
#endif

		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
		glfwTerminate();
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
		while (!glfwWindowShouldClose(s_window))
		{
			frameCount++;			// TODO REMOVE THIS WHEN NOT NEEDED

			// INIT
			// -----

			int w, h;
			glfwGetWindowSize(s_window, &w, &h);

			// input
			// -----
			Input::UpdateInputState();


#pragma region ImGui NewFrame
#if ENABLE_IMGUI == 1
			// ImGui
			// Start a new ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
#endif
#pragma endregion


			// deltaTime
			// ------

			TimePoint currentTime = Clock::now();
			std::chrono::duration<float> delta = currentTime - lastTime;


			//// Sleep if we're early
			//const float targetFrameTime = 1.f / 400.f;
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

			update(delta.count());

			render();


#pragma region ImGui Render

#if ENABLE_IMGUI == 1
			// ImGui

			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#endif
#pragma endregion

			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			glfwSwapBuffers(s_window);
			glfwPollEvents();
		}


		std::chrono::duration<float> totalTime = lastTime - firstTime;
		std::cout << "Total average fps: " << (float)frameCount / totalTime.count();


	}

#pragma endregion

#pragma region Getters

	glm::ivec2 GetWindowSize()
	{
		int w, h;
		glfwGetWindowSize(s_window, &w, &h);
		return glm::ivec2(w, h);
	}

#pragma endregion

#pragma region Setters

	void SetWindowResizeCallback(ResizeCallback callback)
	{
		s_windowResizeCallback = callback;
	}

#pragma endregion

#pragma region Render options


	void SetVsync(bool b)
	{
		glfwSwapInterval(b);
	}

	void SetWireframe(bool b)
	{
		if (b)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void SetApplicationIcon(const std::string& path)
	{
		int width, height, nrChannels;
		unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

		if (pixels)
		{
			GLFWimage images[1];
			images[0].width = width;
			images[0].height = height;
			images[0].pixels = pixels;

			glfwSetWindowIcon(s_window, 1, images);
			stbi_image_free(pixels);
		}
		else
		{
			LogError("SetApplicationIcon: failed to load icon: " + path);
		}
	}

#pragma endregion

#pragma region Internal

	// internal only functions
	namespace 
	{

		//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
		//// ---------------------------------------------------------------------------------------------------------
		//void processInput(GLFWwindow* window)
		//{
		//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		//		glfwSetWindowShouldClose(window, true);
		//}

		// glfw: whenever the window size changed (by OS or user resize) this callback function executes
		// ---------------------------------------------------------------------------------------------
		void framebuffer_size_callback(GLFWwindow* window, int width, int height)
		{
			// make sure the viewport matches the new window dimensions; note that width and 
			// height will be significantly larger than specified on retina displays.
			//glViewport(0, 0, width, height);

			if (s_windowResizeCallback)
				s_windowResizeCallback(width, height);
		}

		void PrintStackTrace()
		{
			constexpr int MAX_FRAMES = 64;

#if defined(_WIN32) || defined(_WIN64)
			void* stack[MAX_FRAMES];
			HANDLE process = GetCurrentProcess();

			USHORT frames = CaptureStackBackTrace(0, MAX_FRAMES, stack, nullptr);

			SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256, 1);
			symbol->MaxNameLen = 255;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

			SymInitialize(process, NULL, TRUE);

			std::cerr << "Call stack:\n";
			for (USHORT i = 0; i < frames; ++i) {
				if (SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol)) {
					std::cerr << i << ": " << symbol->Name << " - 0x" << std::hex << symbol->Address << std::dec << "\n";
				}
			}

			free(symbol);

#else
			void* buffer[MAX_FRAMES];
			int nptrs = backtrace(buffer, MAX_FRAMES);
			char** symbols = backtrace_symbols(buffer, nptrs);

			std::cerr << "Call stack:\n";
			for (int i = 0; i < nptrs; ++i) {
				// Try to demangle
				char* mangledName = symbols[i];
				char* demangled = nullptr;
				int status = 0;

				// attempt to demangle
				demangled = abi::__cxa_demangle(mangledName, nullptr, nullptr, &status);
				if (status == 0 && demangled) {
					std::cerr << i << ": " << demangled << "\n";
					free(demangled);
				}
				else {
					std::cerr << i << ": " << symbols[i] << "\n";
				}
			}

			free(symbols);
#endif
		}


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
