cmake_minimum_required(VERSION 3.10)
project(OpenGLPlayground)

set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE "GLVND")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# Include directories for external libraries
include_directories(${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} external/imgui external/glm)

add_executable(playground.app main.cpp)

# Link libraries
target_link_libraries(playground.app ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} GLEW)


