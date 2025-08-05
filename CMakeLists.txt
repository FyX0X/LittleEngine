cmake_minimum_required(VERSION 3.16)
project(LittleEngine)

add_library(LittleEngine STATIC)


add_subdirectory(thirdparty/glfw-3.3.2)			#window oppener
add_subdirectory(thirdparty/glad)				#opengl loader
add_subdirectory(thirdparty/stb_image)			#loading immaged
add_subdirectory(thirdparty/freetype-2.13.3)	#loading ttf files
#add_subdirectory(thirdparty/enet-1.3.17)		#networking
add_subdirectory(thirdparty/miniaudio)			#audio
add_subdirectory(thirdparty/glm)				#math
add_subdirectory(thirdparty/imgui)				#ui

target_sources(LittleEngine PRIVATE 
	"src/little_engine.cpp" 
	"src/Graphics/shader.cpp" 
	"src/Graphics/camera.cpp" 
	"src/Graphics/texture.cpp" 
	"src/Graphics/renderer.cpp" 
	"src/Graphics/font.cpp" 
	"src/Graphics/tilemap_renderer.cpp" 
	"src/Input/input.cpp" 
	"src/Audio/audio.cpp" 
	"src/Audio/sound.cpp" 
	"src/Graphics/render_target.cpp" 
	"src/Graphics/bitmap_helper.cpp" 
	"src/Storage/storage_helper.cpp"
 "include/LittleEngine/Math/geometry.h" "src/Math/geometry.cpp" "src/Graphics/lighting.cpp" "include/LittleEngine/UI/ui_system.h" "src/UI/ui_system.cpp" "include/LittleEngine/UI/ui_element.h" "src/UI/ui_element.cpp" "include/LittleEngine/UI/ui_context.h" "src/UI/ui_context.cpp" "include/LittleEngine/Math/random.h" "src/Math/random.cpp" "include/LittleEngine/Platform/GLFW/glfw_window.h" "src/Platform/GLFW/glfw_window.cpp" "include/LittleEngine/Core/window.h"  "include/LittleEngine/Platform/GLFW/platform_glfw.h" "include/LittleEngine/Platform/platform.h"  "src/Platform/GLFW/platform_glfw.cpp" "src/error_logger.cpp") # add later "src/gl2dParticleSystem.cpp")

target_include_directories(LittleEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(LittleEngine PUBLIC glm glad stb_image freetype glfw imgui miniaudio)