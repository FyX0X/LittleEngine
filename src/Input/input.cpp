
#include "LittleEngine/Input/input.h"

#include "LittleEngine/Utils/logger.h"
#include "LittleEngine/Core/window.h" // for callbacks

#include <glad/glad.h>

#ifdef USE_GLFW

#include <GLFW/glfw3.h>
#endif // USE_GLFW




#if ENABLE_IMGUI == 1
#include <imgui.h>

#ifdef USE_GLFW
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#elif defined(USE_SDL)
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>
#endif // USE_GLFW
#endif // 



#include <unordered_map>
#include <memory>
#include <array>




namespace LittleEngine::Input
{
#pragma region static containers
	constexpr int MaxKeyCode = static_cast<uint16_t>(KeyCode::SIZE);
	constexpr int MaxMouseButton = static_cast<uint8_t>(MouseButton::SIZE);

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

	static int s_windowWidth = 0;
	static int s_windowHeight = 0;

	// virtual axis
	// One vector<string> per keys, allows for multiple axis binded to same key.
	static std::array<std::vector<std::string>, MaxKeyCode> s_posKeyAxisBindings{};
	static std::array<std::vector<std::string>, MaxKeyCode> s_negKeyAxisBindings{};
	static std::unordered_map<std::string, InputAxis> s_axices;

#pragma endregion

#pragma region Initialization management

	void Initialize(GLFWwindow* window, const glm::ivec2& windowSize)
	{
		ResetKeyState();	// just to be sure.

		UpdateWindowSize(windowSize.x, windowSize.y);
	}


	void Shutdown()
	{
	}

#pragma endregion

#pragma region Update

	void UpdateInputState()
	{
		
		// reset all axis;
		for (auto& [name, axis] : s_axices)
		{
			axis = 0.f;
		}

		// Keys
		for (uint16_t i = 0; i < MaxKeyCode; ++i)
		{
			KeyCode key = static_cast<KeyCode>(i);
			bool previous = s_previousKeyState[i];
			bool current = s_currentKeyState[i];
			
			// update axis.
			if (current)
			{
				UpdateAxis(key);
			}

			std::vector<std::unique_ptr<Command>>& cmds = s_keyBindings[i];
			if (cmds.empty()) continue;


			InputEventType type = GetInputEventType(previous, current);


			for (size_t j = 0; j < cmds.size(); j++)
			{
				CallCommand(cmds[j].get(), type);
			}


		}

		// Same for mouse buttons...
		for (uint8_t i = 0; i < MaxMouseButton; ++i)
		{
			MouseButton btn = static_cast<MouseButton>(i);
			bool current = s_currentMouseButtonState[i];
			bool previous = s_previousMouseButtonState[i];


			std::vector<std::unique_ptr<Command>>& cmds = s_mouseButtonBindings[i];
			if (cmds.empty()) continue;

			InputEventType type = GetInputEventType(previous, current);
			
			for (size_t j = 0; j < cmds.size(); j++)
			{
				CallCommand(cmds[j].get(), type);
			}
		}

		// clamp axices
		for (auto& [name, axis] : s_axices)
		{
			if (axis > 1)
				axis = 1.f;
			else if (axis < -1)
				axis = -1.f;
		}



		
	}

	void UpdatePreviousInputState()
	{		// for next frame
		s_previousKeyState = s_currentKeyState;
		s_previousMouseButtonState = s_currentMouseButtonState;
	}

	void UpdateWindowSize(int w, int h)
	{
		s_windowWidth = w;
		s_windowHeight = h;
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
			Utils::Logger::Warning("Input::CallCommand: Command is nullptr.");
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
		int keyIndex = static_cast<uint16_t>(key);

		std::vector<std::string>& posAxices = s_posKeyAxisBindings[keyIndex];
		std::vector<std::string>& negAxices = s_negKeyAxisBindings[keyIndex];

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

	glm::ivec2 GetMousePosition()
	{
		return glm::ivec2(s_mouseX, s_mouseY);
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
		Utils::Logger::Warning("Input::GetAxis: No axis with name: " + name);
		return 0;

	}

	bool IsKeyDown(KeyCode key) 
	{
		return s_currentKeyState[static_cast<uint16_t>(key)];
	}

	bool IsKeyPressed(KeyCode key) 
	{
		return s_currentKeyState[static_cast<uint16_t>(key)] && !s_previousKeyState[static_cast<uint16_t>(key)];
	}

	bool IsKeyReleased(KeyCode key) 
	{
		return !s_currentKeyState[static_cast<uint16_t>(key)] && s_previousKeyState[static_cast<uint16_t>(key)];
	}

	bool IsMouseButtonDown(MouseButton mb)
	{
		return s_currentMouseButtonState[static_cast<uint8_t>(mb)];
	}

	bool IsMouseButtonPressed(MouseButton mb)
	{
		return s_currentMouseButtonState[static_cast<uint8_t>(mb)] && !s_previousMouseButtonState[static_cast<uint8_t>(mb)];
	}

	bool IsMouseButtonReleased(MouseButton mb)
	{
		return !s_currentMouseButtonState[static_cast<uint8_t>(mb)] && s_previousMouseButtonState[static_cast<uint8_t>(mb)];
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
			Utils::Logger::Warning("Input::RegisterAxis: Axis '" + name + "' already exists.");
		}
	}

	void BindKeysToAxis(KeyCode keyPositive, KeyCode keyNegative, const std::string& name)
	{

		if (s_axices.find(name) == s_axices.end())	// not found
		{
			Utils::Logger::Warning("Input::BindKeysToAxis: axis '" + name + "' does not exists.");
			return;
		}

		s_posKeyAxisBindings[static_cast<uint16_t>(keyPositive)].push_back(name);
		s_negKeyAxisBindings[static_cast<uint16_t>(keyNegative)].push_back(name);

		
	}

	void BindKeyToCommand(KeyCode key, std::unique_ptr<Command> cmd)
	{
		// check if action already binded to key.
		auto& cmds = s_keyBindings[static_cast<uint16_t>(key)];
		for (size_t i = 0; i < cmds.size(); i++)
		{
			if (typeid(*cmds[i]) == typeid(*cmd))		// *cmd and not cmd.get() to dereference and get Actual type (not Command)
			{
				Utils::Logger::Warning("Input::BindKeyToCommand: Command " + cmd->GetName() + " already binded to key: " + std::to_string(static_cast<uint16_t>(key)));
				return;
			}
		}
		cmds.push_back(std::move(cmd));
	}

	void BindMouseButtonToCommand(MouseButton mb, std::unique_ptr<Command> cmd)
	{
		// check if action already binded to mouse button.
		auto& cmds = s_mouseButtonBindings[static_cast<uint8_t>(mb)];
		for (size_t i = 0; i < cmds.size(); i++)
		{
			if (typeid(*cmds[i]) == typeid(*cmd))
			{
				Utils::Logger::Warning("Input::BindMouseButtonToCommand: command: " + cmd->GetName() + " already binded to mouse button: " + std::to_string(static_cast<uint8_t>(mb)));
				return;
			}
		}
		cmds.push_back(std::move(cmd));
	}

#pragma endregion

#pragma region Callbacks

	void scroll_callback(float xoffset, float yoffset)
	{
		// xoffset and yoffset indicate scroll amount (e.g. vertical scroll in yoffset)
		Utils::Logger::Warning("Input::scroll_callback not implemented yet.");
	}

	void cursor_position_callback(float xpos, float ypos)
	{
		// xpos and ypos are the new mouse cursor position in window coordinates
		// where (0, 0) is the top left corner and measured in pixels.

		s_mouseX = static_cast<int>(xpos);
		s_mouseY = s_windowHeight - static_cast<int>(ypos);
	}

	void mouse_button_callback(MouseButton mb, InputEventType type)
	{
		s_currentMouseButtonState[static_cast<uint8_t>(mb)] = type == InputEventType::Pressed;
	}

	void key_callback(KeyCode key, InputEventType type)
	{ 
		s_currentKeyState[static_cast<uint16_t>(key)] = type == InputEventType::Pressed;
	}

	void focus_callback(bool focused)
	{
		if (!focused)
		{
			ResetKeyState();
		}
	}

#pragma endregion

}