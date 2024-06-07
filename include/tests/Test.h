#pragma once

#include <SDL2/SDL.h>

namespace test {
class Test {
 public:
  Test();
  virtual ~Test();

  virtual void OnEvent(SDL_Event& event);
  virtual void OnRender();
  virtual void OnImGuiRender();
};
}  // namespace test
