cmake_minimum_required(VERSION 3.1)
project(LittleEngine)

add_library(LittleEngine)


add_subdirectory(thirdparty/glfw-3.3.2)			#window oppener
add_subdirectory(thirdparty/glad)				#opengl loader
add_subdirectory(thirdparty/stb_image)			#loading immaged
add_subdirectory(thirdparty/freetype-2.13.3)	#loading ttf files
#add_subdirectory(thirdparty/enet-1.3.17)		#networking
add_subdirectory(thirdparty/miniaudio)			#audio
add_subdirectory(thirdparty/glm)				#math
add_subdirectory(thirdparty/imgui)				#ui

target_sources(LittleEngine PRIVATE "src/little_engine.cpp" "include/LittleEngine/Graphics/shader.h" "src/Graphics/shader.cpp" "include/LittleEngine/error_logger.h" "include/LittleEngine/Graphics/camera.h" "src/Graphics/camera.cpp" "include/LittleEngine/Graphics/texture.h" "include/LittleEngine/internal.h" "src/Graphics/texture.cpp" "include/LittleEngine/Graphics/renderer.h" "src/Graphics/renderer.cpp" "include/LittleEngine/Graphics/color.h" "include/LittleEngine/Graphics/font.h" "src/Graphics/font.cpp" "include/LittleEngine/Graphics/default_font.h" "src/Graphics/tilemap_renderer.cpp" "src/Input/input.cpp" "src/Audio/audio.cpp" "src/Audio/sound.cpp" "include/LittleEngine/Audio/sound.h" "include/LittleEngine/Graphics/render_target.h" "src/Graphics/render_target.cpp" "src/Graphics/bitmap_helper.cpp" "include/LittleEngine/Graphics/bitmap_helper.h" "include/LittleEngine/Storage/storage_helper.h" "src/Storage/storage_helper.cpp") # add later "src/gl2dParticleSystem.cpp")
target_include_directories(LittleEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(LittleEngine PUBLIC glm glad stb_image freetype glfw imgui miniaudio)