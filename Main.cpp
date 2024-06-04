#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <iostream>

#include "DataObject.h"
#include "ShaderClass.h"
#include "Window.h"

// Global variables
const char *WINDOW_TITLE = "OpenGL Demo";
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const char *VERT_SHADER_PATH = "./shaders/default_vert.glsl";
const char *FRAG_SHADER_PATH = "./shaders/default_frag.glsl";

// Vertices coordinates
GLfloat vertices[] = {
    -0.5f, -0.5f * float(sqrt(3)) / 3,    0.0f,  // Lower left corner
    0.5f,  -0.5f * float(sqrt(3)) / 3,    0.0f,  // Lower right corner
    0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f   // Upper corner
};

GLuint indices[] = {0, 1, 2};

// Render function
void render(SDL_Window *window, Shader shaderProgram, VAO vao, VBO vbo, EBO ebo) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  shaderProgram.use();
  vao.bind();
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

  SDL_GL_SwapWindow(window);
}

int main(int argc, char *args[]) {
  SDL_Window *window = nullptr;
  SDL_GLContext context;

  if (!initWindow(&window, &context, WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    std::cerr << "Failed to initialize!" << std::endl;
    exit(1);
  }

  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    std::cerr << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
    exit(1);
  }

  bool quit = false;
  SDL_Event e;

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  Shader shaderProgram(VERT_SHADER_PATH, FRAG_SHADER_PATH);

  VAO vao;
  vao.bind();

  VBO vbo(vertices, sizeof(vertices));
  EBO ebo(indices, sizeof(indices));

  vao.linkVBO(vbo, 0);

  vao.unbind();
  vbo.unbind();
  ebo.unbind();

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q) {
        quit = true;
      }
    }

    render(window, shaderProgram, vao, vbo, ebo);
  }

  vao.del();
  vbo.del();
  ebo.del();
  shaderProgram.del();
  close(window, context);

  return 0;
}
