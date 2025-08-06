#include "LittleEngine/Graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "LittleEngine/Utils/logger.h"

namespace LittleEngine::Graphics
{

	void Camera::Follow(const glm::vec2& target, float dt, float speed, float maxDistance)
	{	


		//LERP
		float t = glm::clamp(speed * dt, 0.f, 1.f);
		position = glm::mix(position, target, t);
		return;


		// check max distance
		glm::vec2 delta = target - position;
		float distance = glm::length(delta);
		if (distance > maxDistance)
		{
			position = target + glm::normalize(delta) * maxDistance;
		}

	}



	void Camera::FollowSpring(const glm::vec2& target, float dt, float maxDistance, float stiffness, float damping)
	{
		if (stiffness < 0.f)
		{
			Utils::Logger::Error("Camera::FollowSpring: stiffness was negative: " + std::to_string(stiffness) + ", using 1 instead");
			stiffness = 1.f;		// default stiffness
		}

		if (damping < 0.f)		// for critical damping.
			damping = 2 * glm::sqrt(stiffness);


		// critically damped spring
		glm::vec2 delta = position - target;			// anchor is target, strectched to position
		glm::vec2 acceleration = - stiffness * delta - damping * velocity;		// m = 1 kg

		velocity += acceleration * dt;
		position += velocity * dt;

		// check max distance
		//float distance = glm::length(delta);
		//if (distance > maxDistance)
		//{
		//	std::cout << "Camera::FollowSpring: max distance exceeded: " << distance << " > " << maxDistance << std::endl;
		//	position = target + glm::normalize(delta) * maxDistance;
		//}


	}

	glm::vec2 Camera::ScreenToWorld(const glm::vec2& screenPos) const
	{

		glm::vec2 ndc;
		ndc.x = (2.0f * screenPos.x) / viewportSize.x - 1.0f;
		ndc.y = ((2.0f * screenPos.y) / viewportSize.y - 1.0f);
		// Homogeneous clip space (z=0, w=1 for 2D)
		glm::vec4 clipPos(ndc, 0.0f, 1.0f);
		// Compute inverse view-projection
		glm::mat4 invViewProj = glm::inverse(GetProjectionMatrix() * GetViewMatrix());
		// Transform to world space
		glm::vec4 worldPos = invViewProj * clipPos;

		return glm::vec2(worldPos.x, worldPos.y) / worldPos.w; // divide by w to get the correct position
		
	}


	glm::mat4 Camera::GetProjectionMatrix() const
	{
		float w = viewportSize.x;
		float h = viewportSize.y;

		if (centered) {
			return glm::ortho(-w / 2.0f, w / 2.0f,
				-h / 2.0f, h / 2.0f,
				-1.f, 1.f);
		}
		else {
			return glm::ortho(0.0f, w,
				0.0f, h,
				-1.f, 1.f);
		}
	}

	glm::mat4 Camera::GetViewMatrix() const
	{
		glm::mat4 view = glm::mat4(1.f);
		view = glm::scale(view, glm::vec3(zoom, zoom, 1.f));          // scale first
		view = glm::rotate(view, glm::radians(rotation), glm::vec3(0, 0, -1));
		view = glm::translate(view, glm::vec3(-position, 0.f));       // then translate
		return view;
	}
}
