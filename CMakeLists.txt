cmake_minimum_required(VERSION 3.16)
project(LittleEngine)

add_library(LittleEngine STATIC)


message(STATUS "PLATFORM = '${PLATFORM}'")

# If platform is GLFW, include GLFW
if(PLATFORM STREQUAL "GLFW")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    add_subdirectory(thirdparty/glfw-3.3.2)			#window oppener
    target_compile_definitions(LittleEngine PUBLIC GLFW_INCLUDE_NONE=1) # Prevent GLFW from including OpenGL headers
    target_compile_definitions(LittleEngine PUBLIC USE_GLFW)

elseif(PLATFORM STREQUAL "SDL")
    # Skip GLFW, maybe configure SDL instead
    set(SDL2_STATIC_LIBRARIES ON)
    find_package(SDL2 REQUIRED)
    # Include SDL headers for your engine and game
    target_include_directories(LittleEngine PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(LittleEngine PUBLIC ${SDL2_LIBRARIES})

    target_compile_definitions(LittleEngine PUBLIC USE_SDL)
endif()

add_subdirectory(thirdparty/glad)				#opengl loader
add_subdirectory(thirdparty/stb_image)			#loading immaged
add_subdirectory(thirdparty/freetype-2.13.3)	#loading ttf files
#add_subdirectory(thirdparty/enet-1.3.17)		#networking
add_subdirectory(thirdparty/miniaudio)			#audio
add_subdirectory(thirdparty/glm)				#math
add_subdirectory(thirdparty/imgui)				#ui

# set the target source files
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
target_sources(LittleEngine PRIVATE ${MY_SOURCES})

target_include_directories(LittleEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(LittleEngine PUBLIC glm glad stb_image freetype glfw imgui miniaudio)