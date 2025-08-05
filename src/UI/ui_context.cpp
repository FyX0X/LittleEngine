#include "LittleEngine/UI/ui_context.h"


namespace LittleEngine::UI
{

	UIElement* UIContext::AddElement(std::unique_ptr<UIElement> element)
	{
		m_elements.push_back(std::move(element));
		return m_elements.back().get(); // return the raw pointer to the added element
	}

	bool UIContext::DeleteElement(UIElement* element)
	{
		auto it = std::remove_if(m_elements.begin(), m_elements.end(),
			[element](const std::unique_ptr<UIElement>& e) { return e.get() == element; });

		if (it != m_elements.end())		// found
		{
			m_elements.erase(it, m_elements.end());
			return true;
		}
		return false; // element not found
	}

	void UIContext::Update(const glm::vec2& mousePos, bool mousePressed)
	{
		for (auto& element : m_elements)
		{
			element->Update(mousePos, mousePressed);
		}

	}

	void UIContext::Draw(Graphics::Renderer* renderer) const
	{
		for (const auto& element : m_elements)
		{
			if (!element->IsEnabled()) continue; // skip if context is not enabled and element is not enabled
			element->Draw(renderer);
		}
	}

	void UIContext::SetEnabled(bool enabled)
	{
		m_enabled = enabled;
		for (auto& element : m_elements)
		{
			element->SetEnabled(enabled); // propagate the enabled state to all elements
		}
	}

} // namespace LittleEngine::UI