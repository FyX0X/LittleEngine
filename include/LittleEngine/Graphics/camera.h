#pragma once

#include <glm/glm.hpp>


namespace LittleEngine::Graphics
{


	//used to change the view.
	//whenever you render something, it will be
	//rendered relative to the current camera position.
	//so you can render 2 different things in the same frame at different camera positions.
	//(you will do that for ui for example that you will want to draw with the camera at 0 0).
	struct Camera
	{
		glm::vec2 position = {};
		glm::vec2 velocity = {};

		// Camera rotation (roll) in degrees
		float rotation = 0.f;

		// Camera zoom (scaling), should be 1.0f by default
		float zoom = 1.f;

		void Reset() { *this = Camera{}; }

		//Used to follow objects (player for example).
		//The followed object (pos) will get placed in the center of the screen.
		//maxDistance is the maximum possible distance.
		//min

		/**
		 * Makes the camera follow the given position.
		 * 
		 * currently jitters if fps is not stable
		 * 
		 * @param: pos, the position to follow (will be center of screen)
		 * @param: dt, the frame delta time.
		 * @param: speed, the speed at which the camera follows the position
		 * @param: maxDistance, if the camera is further than maxDistance away from pos, it gets "clamped" back to fit.
		 * @param: minDistance, if the distance is less than minDistance, the camera position gets set to pos.
		 */
		void Follow(const glm::vec2& pos, float dt, float speed = .1f, float maxDistance = 5.f);

		/**
		 * Makes the camera follow the given position using a critically damped spring system.
		 *
		 * @param: pos, the position to follow (will be center of screen)
		 * @param: dt, the frame delta time.
		 * @param: maxDistance, if the camera is further than maxDistance away from pos, it gets "clamped" back to fit.
		 * @param: stiffness, the stifness of the spring.
		 * @param: damping, the damping of the motion (default is critical damping).
		 */
		void FollowSpring(const glm::vec2& target, float dt, float maxDistance = 5.f, float stiffness = 20.f, float damping = -1);

		glm::mat4 GetViewMatrix();
	};


}