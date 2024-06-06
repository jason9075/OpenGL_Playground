#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"

class GUI {
 public:
  GUI(SDL_Window *window, SDL_GLContext context);
  void draw();
  void shutdown();
};
