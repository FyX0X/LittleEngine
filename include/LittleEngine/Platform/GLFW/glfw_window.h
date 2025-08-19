#pragma once


#include "LittleEngine/Core/window.h"


	
// Forward declaration of GLFWwindow
struct GLFWwindow;


namespace LittleEngine::Platform
{


	class GlfwWindow : public LittleEngine::Window
	{
	public:
		bool Initialize(const WindowConfig& config) override;
		void Close() override;


		void OnUpdate() override;


		// =======
		// Setters
		void SetWindowTitle(const std::string& title) override;
		void SetVsync(bool enabled) override;
		void SetWindowMode(const WindowMode& mode) override;
		void SetIcon(const std::string& path) override;


		// =======
		// Getters
		bool IsInitialized() const { return m_window != nullptr; }
		void GetWindowSize(int& width, int& height) const { width = m_state.width; height = m_state.height; }
		bool ShouldClose() const override;
		void* GetNativeWindowHandle() override { return static_cast<void*>(m_window); } // returns the GLFWwindow*
		void* GetNativeContext() override { return nullptr; } //not applicable for GLFW, returns nullptr

		// =========
		// Callbacks
		void SetWindowResizeCallback(Callbacks::Window::Resize callback);
		void SetWindowFocusCallback(Callbacks::Window::Focus callback);
		void SetWindowCloseCallback(Callbacks::Window::Close callback);
		void SetMouseMoveCallback(Callbacks::Mouse::Move callback);
		void SetMouseScrollCallback(Callbacks::Mouse::Scroll callback);
		void SetMouseButtonCallback(Callbacks::Mouse::Button callback);
		void SetKeyCallback(Callbacks::Keyboard::Key callback);
		void SetCharCallback(Callbacks::Keyboard::Char callback);

	private:

		void InitializeCallbacks();


		GLFWwindow* m_window = nullptr; // GLFW window handle
		WindowState m_state = {};


	};


}