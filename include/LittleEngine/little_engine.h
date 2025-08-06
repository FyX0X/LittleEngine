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
#include "LittleEngine/Graphics/lighting.h"
#include "LittleEngine/Input/input.h"
#include "LittleEngine/Audio/audio.h"
#include "LittleEngine/Audio/sound.h"
#include "LittleEngine/Graphics/render_target.h"
#include "LittleEngine/UI/ui_system.h"

#include "LittleEngine/Math/geometry.h"

#include "LittleEngine/internal.h"


#include "LittleEngine/Utils/logger.h"
#include "LittleEngine/Platform/platform.h"
#include "LittleEngine/Core/window.h"




/**		TODO LIST:
 *
 * UI :
 * position with anchor + offset
 * 9 slice
 * 
 * GRAPHICS:
 * 
 * add sprite (texture + uv)
 * 
 * current shader breaks on linux, seems to not respect correct glsl syntax but works on windows due to 
 *		leniant implementation on intel drivers: texture arrays not used correctly.		
 *		either find new implementation
 *		or use bindless textures ?
 *		or sparse bindless texture arrays ?
 * 
 * current solution: use flat int for texture index. => still does not work on linux.
 * new solution: not using array but 16 samplers variables. ok
 * 
 * fix flush to allow better polygon instead of mutliple quads.
 * 
 * add blit fullscreen quad to render target. ok
 * 
 * add render target and fbo. ok
 * 
 * draw circle ?	=> kinda (draw polygon manually)
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
 draw line ok
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
 * use custom button struct instead of GLFW keycode.
 * 
 * scroll
 * mouse get pos ok
 * picking ??
 * 
 * stored keys ok
 * action removed ok
 * Commands ok
 * callbacks ok
 * InputAxis ok
 * 
 * reset input on window focus/unfocus ok
 * 
 * bind same key to multiple action ok
 * 
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
 * fixed timestep for update ok
 * 
 */



namespace LittleEngine
{



	struct EngineConfig		// could be different than the window config, but for now it is the same
	{
		std::string title = "LittleEngine Game";
		int windowWidth = 800;
		int windowHeight = 600;
		WindowMode windowMode = WindowMode::ResizableWindowed; // default is resizable windowed
		bool maximized = false; // if true, the window will be maximized on startup (only works for resizable windowed mode)
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

	float GetFPS();

	Window* GetWindow();

	void SetVsync(bool b);



};