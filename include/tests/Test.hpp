#pragma once

#include "Camera.hpp"

namespace test {
class Test {
 public:
  Test();
  std::shared_ptr<Camera> camera;
  virtual ~Test();

  // Disable copy
  Test(const Test&) = delete;
  Test& operator=(const Test&) = delete;

  virtual void OnEvent(SDL_Event& event);
  virtual void OnRender();
  virtual void OnReload();
  virtual void OnImGuiRender();
  virtual void OnExit();
};
}  // namespace test
