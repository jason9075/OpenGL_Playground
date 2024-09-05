#pragma once

#include "Camera.hpp"

namespace test {
class Test {
 public:
  Test();
  std::shared_ptr<Camera> camera;
  virtual ~Test();

  virtual void OnEvent(SDL_Event& event);
  virtual void OnRender();
  virtual void OnImGuiRender();
  virtual void OnExit();
};
}  // namespace test
