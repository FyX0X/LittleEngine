#include "LittleEngine/Platform/GLFW/glfw_utils.h"


#include <GLFW/glfw3.h>



namespace LittleEngine::Platform::GlfwUtils
{

	Input::KeyCode KeyFromGlfw(int key)
	{
		switch (key)
		{
            case GLFW_KEY_SPACE: return Input::KeyCode::Space;
            case GLFW_KEY_APOSTROPHE: return Input::KeyCode::Apostrophe;
            case GLFW_KEY_COMMA: return Input::KeyCode::Comma;
            case GLFW_KEY_MINUS: return Input::KeyCode::Minus;
            case GLFW_KEY_PERIOD: return Input::KeyCode::Period;
            case GLFW_KEY_SLASH: return Input::KeyCode::Slash;


            // Numbers
            case GLFW_KEY_0: return Input::KeyCode::D0;
            case GLFW_KEY_1: return Input::KeyCode::D1;
            case GLFW_KEY_2: return Input::KeyCode::D2;
		    case GLFW_KEY_3: return Input::KeyCode::D3;
		    case GLFW_KEY_4: return Input::KeyCode::D4;
		    case GLFW_KEY_5: return Input::KeyCode::D5;
		    case GLFW_KEY_6: return Input::KeyCode::D6;
		    case GLFW_KEY_7: return Input::KeyCode::D7;
		    case GLFW_KEY_8: return Input::KeyCode::D8;
		    case GLFW_KEY_9: return Input::KeyCode::D9;

            case GLFW_KEY_SEMICOLON: return Input::KeyCode::Semicolon;
            case GLFW_KEY_EQUAL: return Input::KeyCode::Equal;            // Letters
            case GLFW_KEY_A: return Input::KeyCode::A;
            case GLFW_KEY_B: return Input::KeyCode::B;
            case GLFW_KEY_C: return Input::KeyCode::C;
			case GLFW_KEY_D: return Input::KeyCode::D;
			case GLFW_KEY_E: return Input::KeyCode::E;
			case GLFW_KEY_F: return Input::KeyCode::F;
			case GLFW_KEY_G: return Input::KeyCode::G;
			case GLFW_KEY_H: return Input::KeyCode::H;
			case GLFW_KEY_I: return Input::KeyCode::I;
			case GLFW_KEY_J: return Input::KeyCode::J;
			case GLFW_KEY_K: return Input::KeyCode::K;
			case GLFW_KEY_L: return Input::KeyCode::L;
			case GLFW_KEY_M: return Input::KeyCode::M;   
			case GLFW_KEY_N: return Input::KeyCode::N;
			case GLFW_KEY_O: return Input::KeyCode::O;
			case GLFW_KEY_P: return Input::KeyCode::P;
			case GLFW_KEY_Q: return Input::KeyCode::Q;
			case GLFW_KEY_R: return Input::KeyCode::R;
			case GLFW_KEY_S: return Input::KeyCode::S;
			case GLFW_KEY_T: return Input::KeyCode::T;
			case GLFW_KEY_U: return Input::KeyCode::U;
			case GLFW_KEY_V: return Input::KeyCode::V;
			case GLFW_KEY_W: return Input::KeyCode::W;
			case GLFW_KEY_X: return Input::KeyCode::X;
			case GLFW_KEY_Y: return Input::KeyCode::Y;
			case GLFW_KEY_Z: return Input::KeyCode::Z;

            // Punctuation & symbols
            case GLFW_KEY_LEFT_BRACKET: return Input::KeyCode::LeftBracket;
            case GLFW_KEY_BACKSLASH: return Input::KeyCode::Backslash;
            case GLFW_KEY_RIGHT_BRACKET: return Input::KeyCode::RightBracket;
            case GLFW_KEY_GRAVE_ACCENT: return Input::KeyCode::GraveAccent;
            case GLFW_KEY_WORLD_1: return Input::KeyCode::World1;
            case GLFW_KEY_WORLD_2: return Input::KeyCode::World2;

            // Navigation & misc
            case GLFW_KEY_ESCAPE: return Input::KeyCode::Escape;
            case GLFW_KEY_ENTER: return Input::KeyCode::Enter;
            case GLFW_KEY_TAB: return Input::KeyCode::Tab;
            case GLFW_KEY_BACKSPACE: return Input::KeyCode::Backspace;
            case GLFW_KEY_INSERT: return Input::KeyCode::Insert;
            case GLFW_KEY_DELETE: return Input::KeyCode::Delete;
            case GLFW_KEY_RIGHT: return Input::KeyCode::Right;
            case GLFW_KEY_LEFT: return Input::KeyCode::Left;
            case GLFW_KEY_DOWN: return Input::KeyCode::Down;
            case GLFW_KEY_UP: return Input::KeyCode::Up;
            case GLFW_KEY_PAGE_UP: return Input::KeyCode::PageUp;
            case GLFW_KEY_PAGE_DOWN: return Input::KeyCode::PageDown;
            case GLFW_KEY_HOME: return Input::KeyCode::Home;
            case GLFW_KEY_END: return Input::KeyCode::End;
            case GLFW_KEY_CAPS_LOCK: return Input::KeyCode::CapsLock;
            case GLFW_KEY_SCROLL_LOCK: return Input::KeyCode::ScrollLock;
            case GLFW_KEY_NUM_LOCK: return Input::KeyCode::NumLock;
            case GLFW_KEY_PRINT_SCREEN: return Input::KeyCode::PrintScreen;
            case GLFW_KEY_PAUSE: return Input::KeyCode::Pause;

            // Function keys
            case GLFW_KEY_F1: return Input::KeyCode::F1;
            case GLFW_KEY_F2: return Input::KeyCode::F2;
			case GLFW_KEY_F3: return Input::KeyCode::F3;
			case GLFW_KEY_F4: return Input::KeyCode::F4;
			case GLFW_KEY_F5: return Input::KeyCode::F5;
			case GLFW_KEY_F6: return Input::KeyCode::F6;
			case GLFW_KEY_F7: return Input::KeyCode::F7;
			case GLFW_KEY_F8: return Input::KeyCode::F8;
			case GLFW_KEY_F9: return Input::KeyCode::F9;
			case GLFW_KEY_F10: return Input::KeyCode::F10;
			case GLFW_KEY_F11: return Input::KeyCode::F11;
			case GLFW_KEY_F12: return Input::KeyCode::F12;
			case GLFW_KEY_F13: return Input::KeyCode::F13;
			case GLFW_KEY_F14: return Input::KeyCode::F14;
			case GLFW_KEY_F15: return Input::KeyCode::F15;
			case GLFW_KEY_F16: return Input::KeyCode::F16;
			case GLFW_KEY_F17: return Input::KeyCode::F17;
			case GLFW_KEY_F18: return Input::KeyCode::F18;
			case GLFW_KEY_F19: return Input::KeyCode::F19;
			case GLFW_KEY_F20: return Input::KeyCode::F20;
			case GLFW_KEY_F21: return Input::KeyCode::F21;
			case GLFW_KEY_F22: return Input::KeyCode::F22;
			case GLFW_KEY_F23: return Input::KeyCode::F23;
			case GLFW_KEY_F24: return Input::KeyCode::F24;
			case GLFW_KEY_F25: return Input::KeyCode::F25;

            // Numpad
            case GLFW_KEY_KP_0: return Input::KeyCode::KP0;
            case GLFW_KEY_KP_1: return Input::KeyCode::KP1;
            case GLFW_KEY_KP_2: return Input::KeyCode::KP2;
			case GLFW_KEY_KP_3: return Input::KeyCode::KP3;
			case GLFW_KEY_KP_4: return Input::KeyCode::KP4;
			case GLFW_KEY_KP_5: return Input::KeyCode::KP5;
			case GLFW_KEY_KP_6: return Input::KeyCode::KP6;	
			case GLFW_KEY_KP_7: return Input::KeyCode::KP7;
			case GLFW_KEY_KP_8: return Input::KeyCode::KP8;
			case GLFW_KEY_KP_9: return Input::KeyCode::KP9;

            case GLFW_KEY_KP_DECIMAL: return Input::KeyCode::KPDecimal;
            case GLFW_KEY_KP_DIVIDE: return Input::KeyCode::KPDivide;
            case GLFW_KEY_KP_MULTIPLY: return Input::KeyCode::KPMultiply;
            case GLFW_KEY_KP_SUBTRACT: return Input::KeyCode::KPSubtract;
            case GLFW_KEY_KP_ADD: return Input::KeyCode::KPAdd;
            case GLFW_KEY_KP_ENTER: return Input::KeyCode::KPEnter;
            case GLFW_KEY_KP_EQUAL: return Input::KeyCode::KPEqual;

            // Modifiers
            case GLFW_KEY_LEFT_SHIFT: return Input::KeyCode::LeftShift;
            case GLFW_KEY_LEFT_CONTROL: return Input::KeyCode::LeftControl;
            case GLFW_KEY_LEFT_ALT: return Input::KeyCode::LeftAlt;
            case GLFW_KEY_LEFT_SUPER: return Input::KeyCode::LeftSuper;
            case GLFW_KEY_RIGHT_SHIFT: return Input::KeyCode::RightShift;
            case GLFW_KEY_RIGHT_CONTROL: return Input::KeyCode::RightControl;
            case GLFW_KEY_RIGHT_ALT: return Input::KeyCode::RightAlt;
            case GLFW_KEY_RIGHT_SUPER: return Input::KeyCode::RightSuper;
            case GLFW_KEY_MENU: return Input::KeyCode::Menu;

			default: return Input::KeyCode::Unknown;


		};

	}



	Input::MouseButton MouseButtonFromGlfw(int button)
	{
		if (button < 0 || button > GLFW_MOUSE_BUTTON_8)
			return Input::MouseButton::Unknown; // Invalid button

		return static_cast<Input::MouseButton>(button - GLFW_MOUSE_BUTTON_1 + static_cast<int>(Input::MouseButton::Button0));
	}	
}