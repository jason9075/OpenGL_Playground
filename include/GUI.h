#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

class GUI {
 public:
  GUI(SDL_Window *window, SDL_GLContext context);
  void draw();
  void shutdown();
};
