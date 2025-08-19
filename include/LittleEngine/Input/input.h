#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <glm/glm.hpp>



struct GLFWwindow; // Forward declaration of GLFWwindow

namespace LittleEngine::Input
{
	enum class KeyCode : uint16_t {		// uint16_t is used for future proofing, maybe more keys will be added in the future.
		Unknown = 0, // Used for unrecognized keys
		Space,
		Apostrophe, /* ' */
		Comma, /* , */
		Minus, /* - */
		Period, /* . */
		Slash, /* / */

		D0, /* 0 */
		D1, /* 1 */
		D2, /* 2 */
		D3, /* 3 */
		D4, /* 4 */
		D5, /* 5 */
		D6, /* 6 */
		D7, /* 7 */
		D8, /* 8 */
		D9, /* 9 */

		Semicolon, /* ; */
		Equal, /* = */

		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		LeftBracket,  /* [ */
		Backslash,  /* \ */
		RightBracket,  /* ] */
		GraveAccent,  /* ` */

		World1, /* non-US #1 */
		World2, /* non-US #2 */

		/* Function keys */
		Escape,
		Enter,
		Tab,
		Backspace,
		Insert,
		Delete,
		Right,
		Left,
		Down,
		Up,
		PageUp,
		PageDown,
		Home,
		End,
		CapsLock,
		ScrollLock,
		NumLock,
		PrintScreen,
		Pause,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,

		/* Keypad */
		KP0,
		KP1,
		KP2,
		KP3,
		KP4,
		KP5,
		KP6,
		KP7,
		KP8,
		KP9,
		KPDecimal,
		KPDivide,
		KPMultiply,
		KPSubtract,
		KPAdd,
		KPEnter,
		KPEqual,

		LeftShift,
		LeftControl,
		LeftAlt,
		LeftSuper,
		RightShift,
		RightControl,
		RightAlt,
		RightSuper,
		Menu,
		SIZE,
	};

	enum class MouseButton : uint8_t
	{
		Unknown = 0,	// Used for unrecognized mouse buttons.

		Button0,
		Button1,
		Button2,
		Button3,
		Button4,
		Button5,
		Button6,
		Button7,
		
		SIZE,

		// Aliases for convenience
		Left = Button0,
		Right = Button1,
		Middle = Button2
	};





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


	void scroll_callback(float xoffset, float yoffset);
	void cursor_position_callback(float xpos, float ypos);
	void mouse_button_callback(MouseButton mb, InputEventType type);
	void key_callback(KeyCode key, InputEventType type);
	void focus_callback(bool focused);



}