#include "LittleEngine/Graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "LittleEngine/error_logger.h"

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
			ThrowError("Camera::FollowSpring: stiffness was negative: " + std::to_string(stiffness));
		}

		if (damping < 0.f)		// for critical damping.
			damping = 2 * glm::sqrt(stiffness);

		// critically damped spring
		glm::vec2 delta = position - target;			// anchor is target, strectched to position
		glm::vec2 acceleration = - stiffness * delta - damping * velocity;		// m = 1 kg

		velocity += acceleration * dt;
		position += velocity * dt;

		// check max distance
		float distance = glm::length(delta);
		if (distance > maxDistance)
		{
			position = target + glm::normalize(delta) * maxDistance;
		}


	}

	glm::mat4 Camera::GetViewMatrix()
	{
		glm::mat4 view = glm::mat4(1.f);
		view = glm::scale(view, glm::vec3(zoom, zoom, 1.f));          // scale first
		view = glm::rotate(view, glm::radians(rotation), glm::vec3(0, 0, -1));
		view = glm::translate(view, glm::vec3(-position, 0.f));       // then translate
		return view;
	}
}
