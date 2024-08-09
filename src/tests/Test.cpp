#include "tests/Test.h"

#include <OPPCH.h>

namespace test {

Test::Test() {
  // Default OpenGL settings
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}
Test::~Test() {}

void Test::OnEvent(SDL_Event& event) {}
void Test::OnRender() {}
void Test::OnImGuiRender() {}
void Test::OnExit() {}

}  // namespace test
