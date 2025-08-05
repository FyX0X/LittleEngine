#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <glm/glm.hpp>



struct GLFWwindow; // Forward declaration of GLFWwindow

namespace LittleEngine::Input
{
	using KeyCode = int;
	using MouseButton = int;



	// represents a float in [-1, 1]
	using InputAxis = float;

	class Command {
	public:
		virtual std::string GetName() const = 0;
		virtual void OnPress() {};
		virtual void OnRelease() {};
		virtual void OnHold() {};
		virtual ~Command() = default;
	};




	enum class InputEventType {
		Pressed,   // Trigger once when the key/button goes down
		Released,  // Trigger once when the key/button goes up
		Down,       // Trigger continuously every frame while held
		Up
	};

	// TODO: check if this could be used instead of InputEventType
	//enum class InputCallbackType { Pressed, Released };
	//enum class InputStateType { Down, Up };


	void Initialize(GLFWwindow* window, const glm::ivec2& windowSize);
	void Shutdown();

	void UpdateInputState();
	void UpdatePreviousInputState();
	void UpdateWindowSize(int w, int h);

	InputEventType GetInputEventType(bool previous, bool current);
	void CallCommand(Command* cmd, InputEventType type);
	void UpdateAxis(KeyCode key);

	void ResetKeyState();

	// Returns the current mouse position in pixels
	// (0, 0) is the bottom left corner of the window.
	glm::ivec2 GetMousePosition();
	

	float GetAxis(const std::string& name);
	bool IsKeyDown(KeyCode key);
	bool IsKeyPressed(KeyCode key);
	bool IsKeyReleased(KeyCode key);
	bool IsMouseButtonDown(MouseButton mb);
	bool IsMouseButtonPressed(MouseButton mb);
	bool IsMouseButtonReleased(MouseButton mb);


	void RegisterAxis(const std::string& name);
	void BindKeysToAxis(KeyCode keyPositive, KeyCode keyNegative, const std::string& name);
	void BindKeyToCommand(KeyCode key, std::unique_ptr<Command> cmd);
	void BindMouseButtonToCommand(MouseButton mb, std::unique_ptr<Command> cmd);


	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void focus_callback(GLFWwindow* window, int focused);



}