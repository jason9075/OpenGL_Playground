#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <iostream>

#include "Camera.h"
#include "DataObject.h"
#include "GUI.h"
#include "Mesh.h"
#include "ShaderClass.h"
#include "Window.h"

// Global variables
const char *WINDOW_TITLE = "OpenGL Demo";
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const char *VERT_SHADER_PATH = "./shaders/default_vert.glsl";
const char *FRAG_SHADER_PATH = "./shaders/default_frag.glsl";

// Vertices coordinates
std::vector<Vertex> vertices = {{{-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f}},
                                {{0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f}},
                                {{0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f}}};

std::vector<GLuint> indices = {0, 1, 2};

// Render function
void render(SDL_Window *window, Shader shaderProgram, Camera camera, Mesh mesh) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaderProgram.use();
  camera.update(shaderProgram);
  mesh.draw(shaderProgram);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

int main(int argc, char *args[]) {
  // init SDL
  SDL_Window *window = nullptr;
  SDL_GLContext context;

  if (!initWindow(&window, &context, WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    std::cerr << "Failed to initialize!" << std::endl;
    exit(1);
  }

  // init glew
  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    std::cerr << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
    exit(1);
  }
  // SDL_GL_SetSwapInterval(0);  // Disable VSync

  // init imgui
  GUI gui(window, context);

  // main func
  bool quit = false;
  SDL_Event e;

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  Shader shaderProgram(VERT_SHADER_PATH, FRAG_SHADER_PATH);

  Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 3.0f);
  CameraEventListener listener(camera);
  camera.setEventListener(&listener);

  Mesh mesh(vertices, indices);

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)) {
        quit = true;
      }
      camera.handle(e);
      // ImGui_ImplSDL2_ProcessEvent(&e);
    }

    camera.moveCamera();

    render(window, shaderProgram, camera, mesh);

    gui.draw();

    SDL_GL_SwapWindow(window);
  }

  mesh.del();
  shaderProgram.del();

  gui.shutdown();

  close(window, context);

  return 0;
}
