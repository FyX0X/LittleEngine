#pragma once

#include <string>
#include <functional>
#include "LittleEngine/Input/input.h"


namespace LittleEngine
{

	// THIS IS API ONLY
	// INTERFACE FOR A WINDOW

	namespace Callbacks
	{
		namespace Window
		{
			using Resize = std::function<void(int width, int height)>;
			using Focus = std::function<void(bool focused)>;
			using Close = std::function<void()>;
		}
		namespace Mouse
		{
			using Move = std::function<void(float x, float y)>;
			using Scroll = std::function<void(float xoffset, float yoffset)>;
			using Button = std::function<void(Input::MouseButton button, Input::InputEventType type)>;
		}
		namespace Keyboard
		{
			using Key = std::function<void(Input::KeyCode key, Input::InputEventType type)>;
			using Char = std::function<void(unsigned int codepoint)>;
		}
	}


	enum class WindowMode
	{
		ResizableWindowed,
		FixedWindowed,
		Fullscreen,
		BorderlessFullscreen
	};


	struct WindowConfig
	{
		std::string title = "LittleEngine Window";
		unsigned int width = 1280;
		unsigned int height = 720;
		WindowMode mode = WindowMode::ResizableWindowed;	// current window mode
		bool vsyncEnabled = true;	// whether vsync is enabled or not
		bool maximized = false;	// whether the window is maximized or not
		std::string iconPath = ""; // path to the application icon, can be empty
	};

	struct WindowState
	{
		std::string title;
		int width;
		int height;
		WindowMode mode;
		bool vsyncEnabled;

		// =========
		// Callbacks
		Callbacks::Window::Resize windowResizeCallback = nullptr;
		Callbacks::Window::Focus windowFocusCallback = nullptr;
		Callbacks::Window::Close windowCloseCallback = nullptr;

		Callbacks::Mouse::Move mouseMoveCallback = nullptr;
		Callbacks::Mouse::Scroll mouseScrollCallback = nullptr;
		Callbacks::Mouse::Button mouseButtonCallback = nullptr;

		Callbacks::Keyboard::Key keyCallback = nullptr;
		Callbacks::Keyboard::Char charCallback = nullptr;


		void Reset()
		{
			title = "Default Title";
			width = 1280;
			height = 720;
			mode = WindowMode::ResizableWindowed;
			vsyncEnabled = true;
			windowResizeCallback = nullptr;
			windowFocusCallback = nullptr;
			windowCloseCallback = nullptr;
			mouseMoveCallback = nullptr;
			mouseScrollCallback = nullptr;
			mouseButtonCallback = nullptr;
			keyCallback = nullptr;
			charCallback = nullptr;
		}
	};
	

	class Window
	{
	public:

		virtual ~Window() = default;
		virtual bool Initialize(const WindowConfig& config) = 0;
		virtual void Close() = 0;

		virtual void OnUpdate() = 0;


		// =======
		// Setters
		virtual void SetWindowTitle(const std::string& title) = 0;
		virtual void SetVsync(bool enabled) = 0;
		virtual void SetWindowMode(const WindowMode& mode) = 0;
		virtual void SetIcon(const std::string& path) = 0;


		// =======
		// Getters
		virtual bool IsInitialized() const = 0;
		virtual void GetWindowSize(int& width, int& height) const = 0;
		virtual bool ShouldClose() const = 0;
		virtual void* GetNativeWindowHandle() = 0; // returns the native window handle, if applicable (e.g. GLFWwindow* for GLFW)
		virtual void* GetNativeContext() = 0; // returns the native context handle, if applicable (e.g. SDL_GLContext for SDL2)


		// =========
		// Callbacks
		virtual void SetWindowResizeCallback(Callbacks::Window::Resize callback) = 0;
		virtual void SetWindowFocusCallback(Callbacks::Window::Focus callback) = 0;
		virtual void SetWindowCloseCallback(Callbacks::Window::Close callback) = 0;
		virtual void SetMouseMoveCallback(Callbacks::Mouse::Move callback) = 0;
		virtual void SetMouseScrollCallback(Callbacks::Mouse::Scroll callback) = 0;
		virtual void SetMouseButtonCallback(Callbacks::Mouse::Button callback) = 0;
		virtual void SetKeyCallback(Callbacks::Keyboard::Key callback) = 0;
		virtual void SetCharCallback(Callbacks::Keyboard::Char callback) = 0;

	};

}