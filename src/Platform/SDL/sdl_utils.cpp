#include "LittleEngine/Platform/SDL/sdl_utils.h"


#include <SDL3/SDL.h>

namespace LittleEngine::Platform::SdlUtils
{

	Input::KeyCode KeyFromSdl(SDL_Keycode sdlKey)
	{
		switch (sdlKey)
		{
		case SDLK_SPACE: return Input::KeyCode::Space;
		case SDLK_APOSTROPHE: return Input::KeyCode::Apostrophe;
		case SDLK_COMMA: return Input::KeyCode::Comma;
		case SDLK_MINUS: return Input::KeyCode::Minus;
		case SDLK_PERIOD: return Input::KeyCode::Period;
		case SDLK_SLASH: return Input::KeyCode::Slash;


			// Numbers
		case SDLK_0: return Input::KeyCode::D0;
		case SDLK_1: return Input::KeyCode::D1;
		case SDLK_2: return Input::KeyCode::D2;
		case SDLK_3: return Input::KeyCode::D3;
		case SDLK_4: return Input::KeyCode::D4;
		case SDLK_5: return Input::KeyCode::D5;
		case SDLK_6: return Input::KeyCode::D6;
		case SDLK_7: return Input::KeyCode::D7;
		case SDLK_8: return Input::KeyCode::D8;
		case SDLK_9: return Input::KeyCode::D9;

		case SDLK_SEMICOLON: return Input::KeyCode::Semicolon;
		case SDLK_EQUALS: return Input::KeyCode::Equal;

			// Letters
		case SDLK_A: return Input::KeyCode::A;
		case SDLK_B: return Input::KeyCode::B;
		case SDLK_C: return Input::KeyCode::C;
		case SDLK_D: return Input::KeyCode::D;
		case SDLK_E: return Input::KeyCode::E;
		case SDLK_F: return Input::KeyCode::F;
		case SDLK_G: return Input::KeyCode::G;
		case SDLK_H: return Input::KeyCode::H;
		case SDLK_I: return Input::KeyCode::I;
		case SDLK_J: return Input::KeyCode::J;
		case SDLK_K: return Input::KeyCode::K;
		case SDLK_L: return Input::KeyCode::L;
		case SDLK_M: return Input::KeyCode::M;
		case SDLK_N: return Input::KeyCode::N;
		case SDLK_O: return Input::KeyCode::O;
		case SDLK_P: return Input::KeyCode::P;
		case SDLK_Q: return Input::KeyCode::Q;
		case SDLK_R: return Input::KeyCode::R;
		case SDLK_S: return Input::KeyCode::S;
		case SDLK_T: return Input::KeyCode::T;
		case SDLK_U: return Input::KeyCode::U;
		case SDLK_V: return Input::KeyCode::V;
		case SDLK_W: return Input::KeyCode::W;
		case SDLK_X: return Input::KeyCode::X;
		case SDLK_Y: return Input::KeyCode::Y;
		case SDLK_Z: return Input::KeyCode::Z;

			// Punctuation & symbols
		case SDLK_LEFTBRACKET: return Input::KeyCode::LeftBracket;
		case SDLK_BACKSLASH: return Input::KeyCode::Backslash;
		case SDLK_RIGHTBRACKET: return Input::KeyCode::RightBracket;
		case SDLK_GRAVE: return Input::KeyCode::GraveAccent;
		//case SDLK_WORLD_1: return Input::KeyCode::World1;
		//case SDLK_WORLD_2: return Input::KeyCode::World2;		// seems not supported in SDL3

			// Navigation & misc
		case SDLK_ESCAPE: return Input::KeyCode::Escape;
		case SDLK_RETURN: return Input::KeyCode::Enter;
		case SDLK_TAB: return Input::KeyCode::Tab;
		case SDLK_BACKSPACE: return Input::KeyCode::Backspace;
		case SDLK_INSERT: return Input::KeyCode::Insert;
		case SDLK_DELETE: return Input::KeyCode::Delete;
		case SDLK_RIGHT: return Input::KeyCode::Right;
		case SDLK_LEFT: return Input::KeyCode::Left;
		case SDLK_DOWN: return Input::KeyCode::Down;
		case SDLK_UP: return Input::KeyCode::Up;
		case SDLK_PAGEUP: return Input::KeyCode::PageUp;
		case SDLK_PAGEDOWN: return Input::KeyCode::PageDown;
		case SDLK_HOME: return Input::KeyCode::Home;
		case SDLK_END: return Input::KeyCode::End;
		case SDLK_CAPSLOCK: return Input::KeyCode::CapsLock;
		case SDLK_SCROLLLOCK: return Input::KeyCode::ScrollLock;
		case SDLK_NUMLOCKCLEAR: return Input::KeyCode::NumLock;
		case SDLK_PRINTSCREEN: return Input::KeyCode::PrintScreen;
		case SDLK_PAUSE: return Input::KeyCode::Pause;

			// Function keys
		case SDLK_F1: return Input::KeyCode::F1;
		case SDLK_F2: return Input::KeyCode::F2;
		case SDLK_F3: return Input::KeyCode::F3;
		case SDLK_F4: return Input::KeyCode::F4;
		case SDLK_F5: return Input::KeyCode::F5;
		case SDLK_F6: return Input::KeyCode::F6;
		case SDLK_F7: return Input::KeyCode::F7;
		case SDLK_F8: return Input::KeyCode::F8;
		case SDLK_F9: return Input::KeyCode::F9;
		case SDLK_F10: return Input::KeyCode::F10;
		case SDLK_F11: return Input::KeyCode::F11;
		case SDLK_F12: return Input::KeyCode::F12;
		case SDLK_F13: return Input::KeyCode::F13;
		case SDLK_F14: return Input::KeyCode::F14;
		case SDLK_F15: return Input::KeyCode::F15;
		case SDLK_F16: return Input::KeyCode::F16;
		case SDLK_F17: return Input::KeyCode::F17;
		case SDLK_F18: return Input::KeyCode::F18;
		case SDLK_F19: return Input::KeyCode::F19;
		case SDLK_F20: return Input::KeyCode::F20;
		case SDLK_F21: return Input::KeyCode::F21;
		case SDLK_F22: return Input::KeyCode::F22;
		case SDLK_F23: return Input::KeyCode::F23;
		case SDLK_F24: return Input::KeyCode::F24;

			// Numpad
		case SDLK_KP_0: return Input::KeyCode::KP0;
		case SDLK_KP_1: return Input::KeyCode::KP1;
		case SDLK_KP_2: return Input::KeyCode::KP2;
		case SDLK_KP_3: return Input::KeyCode::KP3;
		case SDLK_KP_4: return Input::KeyCode::KP4;
		case SDLK_KP_5: return Input::KeyCode::KP5;
		case SDLK_KP_6: return Input::KeyCode::KP6;
		case SDLK_KP_7: return Input::KeyCode::KP7;
		case SDLK_KP_8: return Input::KeyCode::KP8;
		case SDLK_KP_9: return Input::KeyCode::KP9;

		case SDLK_KP_DECIMAL: return Input::KeyCode::KPDecimal;
		case SDLK_KP_DIVIDE: return Input::KeyCode::KPDivide;
		case SDLK_KP_MULTIPLY: return Input::KeyCode::KPMultiply;
		case SDLK_KP_MINUS: return Input::KeyCode::KPSubtract;
		case SDLK_KP_PLUS: return Input::KeyCode::KPAdd;
		case SDLK_KP_ENTER: return Input::KeyCode::KPEnter;
		case SDLK_KP_EQUALS: return Input::KeyCode::KPEqual;

			// Modifiers
		case SDLK_LSHIFT: return Input::KeyCode::LeftShift;
		case SDLK_LCTRL: return Input::KeyCode::LeftControl;
		case SDLK_LALT: return Input::KeyCode::LeftAlt;
		case SDLK_LGUI: return Input::KeyCode::LeftSuper;
		case SDLK_RSHIFT: return Input::KeyCode::RightShift;
		case SDLK_RCTRL: return Input::KeyCode::RightControl;
		case SDLK_RALT: return Input::KeyCode::RightAlt;
		case SDLK_RGUI: return Input::KeyCode::RightSuper;
		case SDLK_MENU: return Input::KeyCode::Menu;

		default: return Input::KeyCode::Unknown;


		};
	}

	Input::MouseButton MouseButtonFromSdl(Uint8 sdlButton)
	{
		switch (sdlButton)
		{
			case SDL_BUTTON_LEFT: return Input::MouseButton::Left;
			case SDL_BUTTON_RIGHT: return Input::MouseButton::Right;
			case SDL_BUTTON_MIDDLE: return Input::MouseButton::Middle;
			case SDL_BUTTON_X1: return Input::MouseButton::Button3;
			case SDL_BUTTON_X2: return Input::MouseButton::Button4;
			default: return Input::MouseButton::Unknown;
		}
	}

}