#pragma once

#include "tests/Test.h"

class GUI {
 public:
  test::Test*& currentTest;

  GUI(SDL_Window* window, SDL_GLContext context, test::Test*& currentTest);
  void draw();
  void shutdown();
  bool isMouseOverGUI();

  template <typename T, typename... Args>
  void registerTest(const std::string& name, Args&&... args) {
    std::cout << "Registering test " << name << std::endl;

    allTests.push_back(std::make_pair(name, [args...]() { return new T(args...); }));
  }

 private:
  static bool itemGetter(void* data, int idx, const char** out_text);
  std::vector<std::pair<std::string, std::function<test::Test*()>>> allTests;
  int selectedItem = 0;
};
