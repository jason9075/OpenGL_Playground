#include "tests/Test.h"

#include <OPPCH.h>

namespace test {

Test::Test() {}
Test::~Test() {}

void Test::OnEvent(SDL_Event& event) {}
void Test::OnRender() {}
void Test::OnImGuiRender() {}

}  // namespace test
