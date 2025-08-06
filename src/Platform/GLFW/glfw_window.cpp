#include "LittleEngine/Platform/GLFW/glfw_window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image/stb_image.h>
#include <cassert>

#include "LittleEngine/Utils/logger.h"
#include "LittleEngine/Utils/debug_tools.h"



namespace LittleEngine::Platform
{




	bool GlfwWindow::Initialize(const WindowConfig& config)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if DEVELOPMENT_BUILD == 1
		// enable more debugging
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

		if (config.maximized)
			glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

		m_window = glfwCreateWindow(config.width, config.height, config.title.c_str(), NULL, NULL);
		if (m_window == NULL)
		{
			Utils::Logger::Critical("Failed to create GLFW window");
			glfwTerminate();
			return false;
		}


		glfwMakeContextCurrent(m_window);
		// Load OpenGL function pointers using glad
				// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}

#if DEVELOPMENT_BUILD == 1
		// Set the OpenGL debug callback
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(Utils::DebugTools::glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
#endif



		glfwSetWindowUserPointer(m_window, &m_state);
		SetVsync(true);

		// Set initial window mode
		SetWindowMode(config.mode);
		m_state.title = config.title;
		m_state.width = config.width;
		m_state.height = config.height;
		m_state.mode = config.mode;
		m_state.vsyncEnabled = config.vsyncEnabled;

		if (config.maximized)
		{
			glfwGetFramebufferSize(m_window, &m_state.width, &m_state.height);
		}



		// Initialize callbacks
		InitializeCallbacks();


		if (!config.iconPath.empty())
		{
			SetIcon(config.iconPath);
		}


		glDisable(GL_DEPTH_TEST);	// disable depth test by default


		return true;
	}




	void GlfwWindow::Close()
	{

		if (!m_window)
		{
			Utils::Logger::Warning("GlfwWindow::Close: Window not initialized.");
			return;
		}
		glfwDestroyWindow(m_window);
		m_window = nullptr;	// reset window pointer
		m_state = {}; // reset window state

	}

	void GlfwWindow::OnUpdate()
	{
		assert(m_window && "GlfwWindow::OnUpdate: Window not initialized.");		// not checked in release mode, only in debug mode

		glfwPollEvents(); // Poll for and process events
		glfwSwapBuffers(m_window); // Swap front and back buffers
	}



	void GlfwWindow::SetWindowTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_window, title.c_str()); 
		m_state.title = title;
	}

	void GlfwWindow::SetVsync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0); 
		m_state.vsyncEnabled = enabled;
	}

	void GlfwWindow::SetWindowMode(const WindowMode& mode)
	{
		if (mode == m_state.mode)
			return; // no change

		m_state.mode = mode;
		switch (mode)
		{
		case WindowMode::ResizableWindowed:
			break;
		default:
			Utils::Logger::Warning("GlfwWindow::SetWindowMode: Not yet implemented.");
			break;
		}

		
	}

	void GlfwWindow::SetIcon(const std::string& path)
	{
		if (!m_window)
		{
			Utils::Logger::Warning("GlfwWindow::SetIcon: Window not initialized.");
			return;
		}
		int width, height, nrChannels;
		unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

		if (pixels)
		{
			GLFWimage images[1];
			images[0].width = width;
			images[0].height = height;
			images[0].pixels = pixels;

			glfwSetWindowIcon(m_window, 1, images);
			stbi_image_free(pixels);
		}
		else
		{
			Utils::Logger::Warning("GlfwWindow::SetIcon: failed to load icon: " + path);
		}
	}

#pragma region Callbacks
	bool GlfwWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_window); 
	}

	void GlfwWindow::SetWindowResizeCallback(Callbacks::Window::Resize callback)
	{
		assert(m_window && "GlfwWindow::SetWindowResizeCallback: Window not initialized.");
		m_state.windowResizeCallback = callback;
	}

	void GlfwWindow::SetWindowFocusCallback(Callbacks::Window::Focus callback)
	{
		assert(m_window && "GlfwWindow::SetWindowFocusCallback: Window not initialized.");
		m_state.windowFocusCallback = callback;
	}

	void GlfwWindow::SetWindowCloseCallback(Callbacks::Window::Close callback)
	{
		assert(m_window && "GlfwWindow::SetWindowCloseCallback: Window not initialized.");
		m_state.windowCloseCallback = callback;
	}

	void GlfwWindow::SetMouseMoveCallback(Callbacks::Mouse::Move callback)
	{
		assert(m_window && "GlfwWindow::SetMouseMoveCallback: Window not initialized.");
		m_state.mouseMoveCallback = callback;
	}

	void GlfwWindow::SetMouseScrollCallback(Callbacks::Mouse::Scroll callback)
	{
		assert(m_window && "GlfwWindow::SetMouseScrollCallback: Window not initialized.");
		m_state.mouseScrollCallback = callback;
	}

	void GlfwWindow::SetMouseButtonCallback(Callbacks::Mouse::Button callback)
	{
		assert(m_window && "GlfwWindow::SetMouseButtonCallback: Window not initialized.");
		m_state.mouseButtonCallback = callback;
	}

	void GlfwWindow::SetKeyCallback(Callbacks::Keyboard::Key callback)
	{
		assert(m_window && "GlfwWindow::SetKeyCallback: Window not initialized.");
		m_state.keyCallback = callback;
	}

	void GlfwWindow::SetCharCallback(Callbacks::Keyboard::Char callback)
	{
		assert(m_window && "GlfwWindow::SetCharCallback: Window not initialized.");
		m_state.charCallback = callback;
	}


	void GlfwWindow::InitializeCallbacks()
	{
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
			{
				WindowState& state = *static_cast<WindowState*>(glfwGetWindowUserPointer(window));
				state.width = width;
				state.height = height;
				if (state.windowResizeCallback)
					state.windowResizeCallback(width, height);
			});

		glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused)
			{
				WindowState& state = *static_cast<WindowState*>(glfwGetWindowUserPointer(window));
				if (state.windowFocusCallback)
					state.windowFocusCallback(focused == GLFW_TRUE);
			});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
			{
				WindowState& state = *static_cast<WindowState*>(glfwGetWindowUserPointer(window));
				if (state.windowCloseCallback)
					state.windowCloseCallback();
			});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowState& state = *static_cast<WindowState*>(glfwGetWindowUserPointer(window));
				if (state.mouseMoveCallback)
					state.mouseMoveCallback(static_cast<float>(xpos), static_cast<float>(ypos));
			});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				WindowState& state = *static_cast<WindowState*>(glfwGetWindowUserPointer(window));
				if (state.mouseScrollCallback)
					state.mouseScrollCallback(static_cast<float>(xoffset), static_cast<float>(yoffset));
			});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowState& state = *static_cast<WindowState*>(glfwGetWindowUserPointer(window));
				Input::MouseButton mb = static_cast<Input::MouseButton>(button);
				Input::InputEventType type = action == GLFW_PRESS ? Input::InputEventType::Pressed : Input::InputEventType::Released;
				if (state.mouseButtonCallback)
					state.mouseButtonCallback(mb, type);
			});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowState& state = *static_cast<WindowState*>(glfwGetWindowUserPointer(window));
				Input::KeyCode k = static_cast<Input::KeyCode>(key);
				Input::InputEventType type;
				if (action == GLFW_PRESS || action == GLFW_REPEAT)
					type = Input::InputEventType::Pressed;
				else // if (action == GLFW_RELEASE)
					type = Input::InputEventType::Released;

				if (state.keyCallback)
					state.keyCallback(k, type);
			});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint)
			{
				WindowState& state = *static_cast<WindowState*>(glfwGetWindowUserPointer(window));

				if (state.charCallback)
					state.charCallback(codepoint);
			});
	}
#pragma endregion

}