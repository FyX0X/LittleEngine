#pragma once

#include "LittleEngine/Graphics/renderer.h"

#include <functional> 


namespace LittleEngine::UI
{
	class UIElement
	{
	public:
		UIElement() = default;
		~UIElement() = default;


		// mousePressed should be true on press (not hold)
		void Update(const glm::vec2 mousePos, bool mousePressed);
		virtual void Draw(Graphics::Renderer* renderer) const = 0;

		inline bool IsHovered() const { return m_isHovered; }
		inline bool IsClicked() const { return m_isClicked; }

		inline void SetEnabled(bool enabled) { m_enabled = enabled; }



	protected:
		virtual bool ContainsPoint(const glm::vec2& point) const = 0;
		virtual void OnClick() {};	// no op by default;

	private:
		bool m_isHovered = false;	// whether the element is currently hovered by the mouse
		bool m_isClicked = false;	// whether the element is currently clicked by the mouse
		bool m_enabled = false;		// whether the element is enabled or not, can be used to disable interaction
	};

	class UIButton : public UIElement
	{
	public:
		UIButton(const glm::vec2& position, const glm::vec2& size, const std::string& text)
			: m_position(position), m_size(size), m_text(text) {}

		inline void SetOnClickCallback(std::function<void()> callback) { m_onClickCallback = callback; }

		void Draw(Graphics::Renderer* renderer) const override;

	private:
		bool ContainsPoint(const glm::vec2& point) const override;
		void OnClick() override;

		glm::vec2 m_position;
		glm::vec2 m_size;
		std::string m_text;
		std::function<void()> m_onClickCallback = nullptr;
	};


	class UILabel : public UIElement
	{
	public:
		UILabel(const glm::vec2& position, const std::string& text, float fontSize)
			: m_position(position), m_text(text), m_fontSize(fontSize) {
		}

		void Draw(Graphics::Renderer* renderer) const override;

	private:
		bool ContainsPoint(const glm::vec2& point) const override;
		
		glm::vec2 m_position;
		std::string m_text;
		float m_fontSize;
	};

	class UICheckbox : public UIElement
	{
	public:
		UICheckbox(const glm::vec2& position, float size, const std::string& label, bool initialState = false)
			: m_position(position), m_size(size), m_label(label), m_state(initialState) {
		}

		inline void SetOnToggleCallback(std::function<void(bool)> callback) { m_onToggleCallback = callback; }

		void Draw(Graphics::Renderer* renderer) const override;
		bool IsChecked() const { return m_state; }

	private:
		bool ContainsPoint(const glm::vec2& point) const override;
		void OnClick() override;
		
		glm::vec2 m_position;
		float m_size;
		std::string m_label;
		bool m_state; // true for checked, false for unchecked

		std::function<void(bool)> m_onToggleCallback = nullptr;
	};

}