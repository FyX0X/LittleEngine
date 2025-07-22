#include "LittleEngine/Input/input.h"

#include "LittleEngine/error_logger.h"



#if ENABLE_IMGUI == 1
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#endif // 



#include <unordered_map>
#include <memory>
#include <array>


constexpr int MaxKeyCode = GLFW_KEY_LAST;
constexpr int MaxMouseButton = GLFW_MOUSE_BUTTON_LAST;


namespace LittleEngine::Input
{
#pragma region static containers

	// one vector<Command> for each key. allows for multiple command per key.
	static std::array<std::vector<std::unique_ptr<Command>>, MaxKeyCode> s_keyBindings{};
	static std::array<std::vector<std::unique_ptr<Command>>, MaxMouseButton> s_mouseButtonBindings{};


	// Keyboard states
	static std::array<bool, MaxKeyCode> s_currentKeyState{};
	static std::array<bool, MaxKeyCode> s_previousKeyState{};


	// Mouse button states
	static std::array<bool, MaxMouseButton> s_currentMouseButtonState{};
	static std::array<bool, MaxMouseButton> s_previousMouseButtonState{};

	static int s_mouseX = 0;
	static int s_mouseY = 0;

	// virtual axis
	// One vector<string> per keys, allows for multiple axis binded to same key.
	static std::array<std::vector<std::string>, MaxKeyCode> s_posKeyAxisBindings{};
	static std::array<std::vector<std::string>, MaxKeyCode> s_negKeyAxisBindings{};
	static std::unordered_map<std::string, InputAxis> s_axices;

#pragma endregion

#pragma region Initialization management

	void Initialize(GLFWwindow* window)
	{

		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetWindowFocusCallback(window, focus_callback);


		ResetKeyState();	// just to be sure.


	}


	void Shutdown()
	{
	}

#pragma endregion

#pragma region Update

	void UpdateInputState()
	{
		
		// reset all axis;
		for (auto& [key, value] : s_axices)
		{
			value = 0.f;
		}

		//std::cout << "key D: previous (" << s_previousKeyState[68] << "), current (" << s_currentKeyState[68] << ")\n";

		// Keys
		for (int key = 0; key < s_keyBindings.size(); ++key)
		{
			bool previous = s_previousKeyState[key];
			bool current = s_currentKeyState[key];
			
			// update axis.
			if (current)
			{
				UpdateAxis(key);
			}

			std::vector<std::unique_ptr<Command>>& cmds = s_keyBindings[key];
			if (cmds.empty()) continue;


			InputEventType type = GetInputEventType(previous, current);


			for (size_t i = 0; i < cmds.size(); i++)
			{
				CallCommand(cmds[i].get(), type);
			}


		}

		// Same for mouse buttons...
		for (int btn = 0; btn < s_mouseButtonBindings.size(); ++btn)
		{
			bool current = s_currentMouseButtonState[btn];
			bool previous = s_previousMouseButtonState[btn];

			if (btn >= s_mouseButtonBindings.size()) continue;

			std::vector<std::unique_ptr<Command>>& cmds = s_mouseButtonBindings[btn];
			if (cmds.empty()) continue;

			InputEventType type = GetInputEventType(previous, current);
			
			for (size_t i = 0; i < cmds.size(); i++)
			{
				CallCommand(cmds[i].get(), type);
			}
		}

		// clamp axices
		for (auto& [key, value] : s_axices)
		{
			if (value > 1)
				value = 1.f;
			else if (value < -1)
				value = -1.f;
		}

		// for next frame
		s_previousKeyState = s_currentKeyState;
		s_previousMouseButtonState = s_currentMouseButtonState;

		
	}

#pragma endregion

#pragma region Helper

	InputEventType GetInputEventType(bool previous, bool current)
	{
		if (!previous && current)
			return InputEventType::Pressed;

		if (previous && current)
			return InputEventType::Down;

		if (previous && !current)
			return InputEventType::Released;

		return InputEventType::Up;

	}

	void CallCommand(Command* cmd, InputEventType type)
	{

		if (cmd == nullptr)
		{
			LogError("Input::CallCommand: Command is nullptr.");
			return;
		}

		switch (type)
		{
		case LittleEngine::Input::InputEventType::Pressed:
			cmd->OnPress();
			break;
		case LittleEngine::Input::InputEventType::Released:
			cmd->OnRelease();
			break;
		case LittleEngine::Input::InputEventType::Down:
			cmd->OnHold();
			break;
		default:
			//LogWarning("Input::CallAction: unknown input event type.");
			break;
		}

	}

	void UpdateAxis(KeyCode key)
	{
		if (key < 0 || key >= MaxKeyCode)	// just in case
		{
			ThrowError("Input::UpdateAxis: incorrect key: " + std::to_string(key));
		}

		std::vector<std::string>& posAxices = s_posKeyAxisBindings[key];
		std::vector<std::string>& negAxices = s_negKeyAxisBindings[key];

		if (!posAxices.empty())
		{
			for (size_t i = 0; i < posAxices.size(); i++)	// increase each bound axices.
			{
				s_axices[posAxices[i]] += 1.f;
			}
		}
		if (!negAxices.empty())
		{
			for (size_t i = 0; i < negAxices.size(); i++)
			{
				s_axices[negAxices[i]] -= 1.f;
			}
		}
	}

#pragma endregion

#pragma region Setters

	void ResetKeyState()
	{
		std::fill(s_previousKeyState.begin(), s_previousKeyState.end(), false);
		std::fill(s_currentKeyState.begin(), s_currentKeyState.end(), false);
		std::fill(s_previousMouseButtonState.begin(), s_previousMouseButtonState.end(), false);
		std::fill(s_previousMouseButtonState.begin(), s_previousMouseButtonState.end(), false);
	}

#pragma endregion

#pragma region State Getters


	float GetAxis(const std::string& name)
	{
		auto it = s_axices.find(name);
		if (it != s_axices.end())	// found
		{
			return it->second;
		}
		LogError("Input::GetAxis: No axis with name: " + name);
		return 0;

	}

	bool IsKeyDown(KeyCode key) 
	{
		return s_currentKeyState[key];
	}

	bool IsKeyPressed(KeyCode key) 
	{
		return s_currentKeyState[key] && !s_previousKeyState[key];
	}

	bool IsKeyReleased(KeyCode key) 
	{
		return !s_currentKeyState[key] && s_previousKeyState[key];
	}


#pragma endregion

#pragma region Bindings

	void RegisterAxis(const std::string& name)
	{
		auto it = s_axices.find(name);
		if (it == s_axices.end())	// not found
		{
			s_axices[name] = 0;
		}
		else 
		{
			LogError("Input::RegisterAxis: Axis '" + name + "' already exists.");
		}
	}

	void BindKeysToAxis(KeyCode keyPositive, KeyCode keyNegative, const std::string& name)
	{

		if (s_axices.find(name) == s_axices.end())	// not found
		{
			LogError("Input::BindKeysToAxis: axis '" + name + "' does not exists.");
			return;
		}

		s_posKeyAxisBindings[keyPositive].push_back(name);
		s_negKeyAxisBindings[keyNegative].push_back(name);

		
	}

	void BindKeyToCommand(KeyCode key, std::unique_ptr<Command> cmd)
	{
		if (key < 0 || key > MaxKeyCode)
		{
			LogError("Input::BindKeyToCommand: invalid key: " + std::to_string(key));
			return;
		}

		// check if action already binded to key.
		auto& cmds = s_keyBindings[key];
		for (size_t i = 0; i < cmds.size(); i++)
		{
			if (typeid(*cmds[i]) == typeid(*cmd))		// *cmd and not cmd.get() to dereference and get Actual type (not Command)
			{
				LogError("Input::BindKeyToCommand: Command " + cmd->GetName() + " already binded to key: " + std::to_string(key));
				return;
			}
		}
		cmds.push_back(std::move(cmd));
	}

	void BindMouseButtonToCommand(MouseButton mb, std::unique_ptr<Command> cmd)
	{
		if (mb < 0 || mb > MaxMouseButton)
		{
			LogError("Input::BindMouseButtonToCommand: invalid mouse button: " + std::to_string(mb));
			return;
		}
		// check if action already binded to mouse button.
		auto& cmds = s_mouseButtonBindings[mb];
		for (size_t i = 0; i < cmds.size(); i++)
		{
			if (typeid(*cmds[i]) == typeid(*cmd))
			{
				LogError("Input::BindMouseButtonToCommand: command: " + cmd->GetName() + " already binded to mouse button: " + std::to_string(mb));
				return;
			}
		}
		cmds.push_back(std::move(cmd));
	}

#pragma endregion

#pragma region Callbacks

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
#if ENABLE_IMGUI == 1
		// callback for imgui
		ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

#endif
		// xoffset and yoffset indicate scroll amount (e.g. vertical scroll in yoffset)
		LogWarning("Input::scroll_callback not implemented yet.");
	}

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
#if ENABLE_IMGUI == 1
		// callback for imgui
		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

#endif
		// xpos and ypos are the new mouse cursor position in window coordinates
		// where (0, 0) is the top left corner and measured in pixels.

		s_mouseX = static_cast<int>(xpos);
		s_mouseY = static_cast<int>(ypos);
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
#if ENABLE_IMGUI == 1
		// callback for imgui
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);  // Forward to ImGui

#endif
		if (button < 0 || button >= MaxMouseButton) return; // GLFW_MOUSE_BUTTON_LAST = 7

		if (action == GLFW_PRESS)
			s_currentMouseButtonState[button] = true;
		else if (action == GLFW_RELEASE)
			s_currentMouseButtonState[button] = false;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
#if ENABLE_IMGUI == 1
		// callback for imgui
		ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);  // Forward to ImGui

#endif
		if (key < 0 || key >= MaxKeyCode) return; // GLFW_KEY_LAST = 348 (just to be sure)

		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			s_currentKeyState[key] = true;
		else if (action == GLFW_RELEASE)
			s_currentKeyState[key] = false;

	}

	void focus_callback(GLFWwindow* window, int focused)
	{
		if (!focused)
		{
			ResetKeyState();
		}
	}

#pragma endregion

}