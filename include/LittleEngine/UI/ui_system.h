#pragma once

#include <LittleEngine/UI/ui_element.h>
#include <LittleEngine/UI/ui_context.h>
#include "LittleEngine/Graphics/renderer.h"
#include "LittleEngine/Graphics/camera.h"
#include "LittleEngine/Input/input.h"

#include <glm/glm.hpp>
#include <string>
#include <functional>


namespace LittleEngine::UI
{





	class UISystem
	{
	public:
		void Initialize(const glm::ivec2& windowSize);	// initializes the UI system

		void UpdateWindowSize(const glm::ivec2& size) { m_camera.viewportSize = size; }	// sets the viewport size for UI rendering
		void UpdateWindowSize(int w, int h) { m_camera.viewportSize = { w, h }; }	// sets the viewport size for UI rendering

		void Update();		// handles input and updates UI elements
		void Render(Graphics::Renderer* renderer) const;	// draws all UI elements in the context

		UIContext* CreateContext(const std::string& name);	// creates a new UI context
		UIContext* GetContext(const std::string& name);

		void ShowContext(const std::string& name);	// shows a context by name
		void HideContext(const std::string& name);	// hides a context by name
		void ToggleContext(const std::string& name);	// toggles a context by name

	private:
		Graphics::Camera m_camera = {};
		std::unordered_map<std::string, std::unique_ptr<UIContext>> m_contexts;		// list of UI contexts
	};
}