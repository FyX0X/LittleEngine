#include "LittleEngine/UI/ui_element.h"


namespace LittleEngine::UI
{
	void UIElement::Update(const glm::vec2 mousePos, bool mousePressed)
	{
		if (!m_enabled) return; // don't update if the element is not enabled
		m_isHovered = m_enabled && ContainsPoint(mousePos);
		m_isClicked = m_isHovered && mousePressed;
		if (m_isClicked)
			OnClick(); // call the click handler if the element is clicked

	}


	void UIButton::Draw(Graphics::Renderer* renderer) const
	{
		if (!m_enabled) return; // don't draw if the button is not enabled
		renderer->DrawRect(glm::vec4(m_position, m_size), Graphics::Colors::White);
		renderer->DrawString(m_text, m_position + glm::vec2(0, m_size.y), Graphics::Colors::Black, m_size.y * 0.5f);
	}

	bool UIButton::ContainsPoint(const glm::vec2& point) const
	{
		return point.x >= m_position.x && point.x <= m_position.x + m_size.x &&
			point.y >= m_position.y && point.y <= m_position.y + m_size.y;
	}
	void UIButton::OnClick()
	{
		if (m_onClickCallback)
			m_onClickCallback();
	}

	void UILabel::Draw(Graphics::Renderer* renderer) const
	{
		renderer->DrawString(m_text, m_position + glm::vec2(m_fontSize * 0.05f, m_fontSize * 1.05f), Graphics::Colors::Black, m_fontSize);
		renderer->DrawString(m_text, m_position + glm::vec2(0, m_fontSize), Graphics::Colors::White, m_fontSize);
	}
	bool UILabel::ContainsPoint(const glm::vec2& point) const
	{// TODO IMPLEMENT
		return false;
	}

	void UICheckbox::Draw(Graphics::Renderer* renderer) const
	{
		// render the checkbox background
		glm::vec2 box = { m_size, m_size }; // make the checkbox square
		renderer->DrawRect(glm::vec4(m_position, box), Graphics::Colors::Black);
		// render the checkbox inside
		renderer->DrawRect(glm::vec4(m_position + box * 0.1f, box * 0.8f), m_state ? Graphics::Colors::Green : Graphics::Colors::Red);

		// render the label
		renderer->DrawString(m_label, m_position + glm::vec2(m_size * 1.2f, m_size), Graphics::Colors::Black, m_size * 0.5f);
	}
	bool UICheckbox::ContainsPoint(const glm::vec2& point) const
	{
		return point.x >= m_position.x && point.x <= m_position.x + m_size &&
			point.y >= m_position.y && point.y <= m_position.y + m_size;
	}

	void UICheckbox::OnClick()
	{
		m_state = !m_state; // toggle the state
		if (m_onToggleCallback)
			m_onToggleCallback(m_state); // call the toggle callback if set
	}


	void UIRect::Draw(Graphics::Renderer* renderer) const
	{
		renderer->DrawRect(glm::vec4(m_position, m_size), m_texture, m_color, m_uvs);
	}

	bool UIRect::ContainsPoint(const glm::vec2& point) const
	{
		return point.x >= m_position.x && point.x <= m_position.x + m_size.x &&
			point.y >= m_position.y && point.y <= m_position.y + m_size.y;
	}
} // namespace LittleEngine::UI