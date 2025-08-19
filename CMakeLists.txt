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


    file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/GLFW/*.cpp")
    target_sources(LittleEngine PRIVATE ${PLATFORM_SOURCES})

    add_subdirectory(thirdparty/glfw-3.3.2)			#window oppener
    target_compile_definitions(LittleEngine PUBLIC GLFW_INCLUDE_NONE=1) # Prevent GLFW from including OpenGL headers
    target_compile_definitions(LittleEngine PUBLIC USE_GLFW)
    

elseif(PLATFORM STREQUAL "SDL")
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)  # Build static lib
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    
    file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/SDL/*.cpp")
    target_sources(LittleEngine PRIVATE ${PLATFORM_SOURCES})

    add_subdirectory(thirdparty/SDL)
    target_link_libraries(LittleEngine PRIVATE SDL3-static)
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
# remove the platform specific source files from the list
list(FILTER MY_SOURCES EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/.*\\.cpp$")
target_sources(LittleEngine PRIVATE ${MY_SOURCES})

target_include_directories(LittleEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(LittleEngine PUBLIC glm glad stb_image freetype imgui miniaudio)

if(PLATFORM STREQUAL "GLFW")
target_link_libraries(LittleEngine PUBLIC glfw)
elseif(PLATFORM STREQUAL "SDL")
target_link_libraries(LittleEngine PUBLIC SDL3-static)
endif()