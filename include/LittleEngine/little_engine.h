//////////////////////////////////////////////////
// LittleEngine 1.0
// author: Martin Ergo
//////////////////////////////////////////////////

#pragma once

//enable simd functions
//set LittleEngine_SIMD to 0 if it doesn't work on your platform
#ifdef _WIN32
#define LittleEngine_SIMD 1
#else
#define LittleEngine_SIMD 0
#endif


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <random>
#include <stb_image/stb_image.h>
#include <vector>
#include <functional>

#include "LittleEngine/Graphics/camera.h"
#include "LittleEngine/Graphics/color.h"
#include "LittleEngine/Graphics/font.h"
#include "LittleEngine/Graphics/renderer.h"
#include "LittleEngine/Graphics/shader.h"
#include "LittleEngine/Graphics/texture.h"
#include "LittleEngine/Graphics/tilemap_renderer.h"
#include "LittleEngine/Input/input.h"
#include "LittleEngine/Audio/audio.h"
#include "LittleEngine/Audio/sound.h"

#include "LittleEngine/internal.h"



/**		TODO LIST:
 *
 * GRAPHICS:
 * 
 * add render target and fbo. ok but not good yet...
 * maybe still improve. => current projection maybe strange ... because not "pixel unit".
 * draw line
 * draw circle ?
 * draw particle ?
 * optimise renderer flush and batching.	YES -> currently pushing all vertices and indices to flush then tp batch, doing two times the work
 * Optimisation needed.
 * also flush when max quad size is attained.
 * better text rendering. (shadow, higlight)
 * Instancing ? with instanced array ?
 * Padding for texture Atlas ?
 * texture atlas reworked ok (no padding still)
 * Add rotated rects ?
 * Depth/stencil Buffer
 * bloom and light source.
 * 
 * renderer: Flush vs endframe not clear;
 * 
 * write FBO / screen to png ok
 * to take screenshot ok
 * 
 * Querry FPS
 *	
 * INPUT:
 * 
 * stored keys ok
 * action removed
 * Commands ok
 * callbacks ok
 * InputAxis ok
 * 
 * reset input on window focus/unfocus ok
 * 
 * bind same key to multiple action ok
 * 
 * scroll
 * mouse get pos 
 * picking ??
 * 
 * 
 * AUDIO:
 * 
 * basic sound ok
 * background music ?
 * load ogg/mp3/...
 * play same sound multiple time 
 * positionnal audio ok
 * 
 * 
 */



namespace LittleEngine
{

	struct EngineConfig
	{
		std::string title = "LittleEngine Game";
		int windowWidth = 800;
		int windowHeight = 600;
		bool vsync = true;
		std::string iconPath = ""; // path to the application icon, can be empty
	};


	//Initializes the library. Call once before you use the library.
	int Initialize(const EngineConfig& config = {});
	
	//Deinitializes the library.
	void Shutdown();

	// Calls the main loop function from the game
	void Run(const std::function<void(float)>& update, const std::function<void()>& render);


	// Gets the viewport size.
	glm::ivec2 GetWindowSize();


	using ResizeCallback = std::function<void(int width, int height)>;
	void SetWindowResizeCallback(ResizeCallback callback);


	void SetVsync(bool b);
	void SetWireframe(bool b);

	void SetApplicationIcon(const std::string& path);





};