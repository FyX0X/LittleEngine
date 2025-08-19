
#include "LittleEngine/UI/ui_system.h"
#include "LittleEngine/UI/ui_context.h"
#include "LittleEngine/Utils/logger.h"

#ifdef USE_GLFW


// Temporary includes for glad and GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#else
#define GLFW_MOUSE_BUTTON_LEFT 0 // Define GLFW_MOUSE_BUTTON_LEFT if not defined

#endif // USE_GLFW

namespace LittleEngine::UI
{	



#pragma region UIContext

	void UISystem::Initialize(const glm::ivec2& windowSize)
	{
		// Initialize the camera for UI rendering
		m_camera.centered = false; // Center the camera for UI
		UpdateWindowSize(windowSize); // Set the viewport size for UI rendering
	}

	void UISystem::Update()
	{
		glm::vec2 mousePos = Input::GetMousePosition();
		bool mousePressed = Input::IsMouseButtonPressed(Input::MouseButton::Left);

		for (auto& [name, context] : m_contexts)
		{
			context->Update(mousePos, mousePressed);
		}
	}

	void UISystem::Render(Graphics::Renderer* renderer) const
	{
		const Graphics::Camera& oldCamera = renderer->GetCamera();
		renderer->SetCamera(m_camera); // Set the camera for UI rendering

		for (const auto& [name, context] : m_contexts)
		{
			if (context->IsEnabled())
			{
				context->Draw(renderer);
			}
		}
		renderer->Flush();
		renderer->SetCamera(oldCamera); // Restore the previous camera
	}

	UIContext* UISystem::CreateContext(const std::string& name)
	{
		m_contexts[name] = std::make_unique<UIContext>();
		return m_contexts[name].get(); // return the raw pointer to the created context
	}

	UIContext* UISystem::GetContext(const std::string& name)
	{
		auto it = m_contexts.find(name);
		if (it != m_contexts.end())		// found
		{
			return it->second.get();
		}
		Utils::Logger::Warning("UISystem::GetContext: Context not found: " + name);
		return nullptr;

	}

	void UISystem::ShowContext(const std::string& name)
	{
		auto it = m_contexts.find(name);
		if (it != m_contexts.end()) // found
		{
			it->second->SetEnabled(true);
		}
		else // not found
		{
			Utils::Logger::Warning("UISystem::ShowContext: Context not found: " + name);
		}
	}

	void UISystem::HideContext(const std::string& name)
	{
		auto it = m_contexts.find(name);
		if (it != m_contexts.end())
		{
			it->second->SetEnabled(false);
		}
		else
		{
			Utils::Logger::Warning("UISystem::HideContext: Context not found: " + name);
		}
	}

	void UISystem::ToggleContext(const std::string& name)
	{
		auto it = m_contexts.find(name);
		if (it != m_contexts.end())
		{
			it->second->SetEnabled(!it->second->IsEnabled());
		}
		else
		{
			Utils::Logger::Warning("UISystem::ToggleContext: Context not found: " + name);
		}
	}





#pragma endregion
}