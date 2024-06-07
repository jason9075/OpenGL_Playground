cmake_minimum_required(VERSION 3.10)
project(OpenGLPlayground)

set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE "GLVND")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# Include directories for external libraries
include_directories(include/ ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} external/imgui external/glm)

# SDL2 and OpenGL libraries
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/backends/imgui_impl_sdl2.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

file(GLOB SOURCES src/*.cpp src/tests/*.cpp)

# Disable tests and installation for nlohmann_json
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

# Add nlohmann_json subdirectory
add_subdirectory(external/json)

add_executable(playground.app Main.cpp ${SOURCES} ${IMGUI_SOURCES})

# Link libraries
target_link_libraries(playground.app ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} GLEW nlohmann_json::nlohmann_json)


