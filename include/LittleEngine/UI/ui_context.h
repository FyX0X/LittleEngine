#pragma once

#include "LittleEngine/UI/ui_element.h"
#include <memory>

namespace LittleEngine::UI
{


	class UIContext
	{
	public:
		UIContext() = default;
		~UIContext() = default;

		UIElement* AddElement(std::unique_ptr<UIElement> element);
		bool DeleteElement(UIElement* element);
		void ClearElements() { m_elements.clear(); }

		void Update(const glm::vec2& mousePos, bool mousePressed); // handles input and updates UI elements
		void Draw(Graphics::Renderer* renderer) const;

		void SetEnabled(bool enabled); // enable or disable the context
		bool IsEnabled() const { return m_enabled; } // check if the context is enabled
		

	private:
		std::vector<std::unique_ptr<UIElement>> m_elements; // list of UI elements in this context
		bool m_enabled = false; // whether the context is enabled or not, can be used to disable interaction
	};
}