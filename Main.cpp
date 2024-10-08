#include <OPPCH.h>

#include "GUI.hpp"
#include "Window.hpp"
#include "tests/TestCubeMap.hpp"
#include "tests/TestCudaMatMul.hpp"
#include "tests/TestGaussian.hpp"
#include "tests/TestGeometry.hpp"
#include "tests/TestGs.hpp"
#include "tests/TestLighting.hpp"
#include "tests/TestModel.hpp"
#include "tests/TestMultipleObj.hpp"
#include "tests/TestParallaxMapping.hpp"
#include "tests/TestRoom.hpp"
#include "tests/TestRtBVH.hpp"
#include "tests/TestRtSphere.hpp"
#include "tests/TestSdfBlend.hpp"
#include "tests/TestSdfTaipei101.hpp"
#include "tests/TestTriangle.hpp"

// Global variables
const char *WINDOW_TITLE = "OpenGL Demo";
const int SCREEN_WIDTH = 1536;
const int SCREEN_HEIGHT = 864;

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

  // test::Test *currentTest = new test::Test;
  test::Test *currentTest = new test::TestCudaMatMul(SCREEN_WIDTH, SCREEN_HEIGHT);
  GUI gui(window, context, currentTest);

  gui.registerTest<test::Test>("-");  // dummy test
  gui.registerTest<test::TestTriangle>("Triangle", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestModel>("Model", SCREEN_WIDTH, SCREEN_HEIGHT, "./assets/gltf_duck/Duck.gltf");
  gui.registerTest<test::TestMultipleObj>("Multiple Models", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestLighting>("Lighting", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestCubeMap>("CubeMap", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestGs>("Gaussian Splat", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestGaussian>("Gaussian", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestGeometry>("Geometry", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestSdfBlend>("SDF Blend", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestSdfTaipei101>("SDF Taipei101", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestRoom>("Room", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestRtBVH>("RT BVH", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestRtSphere>("RT Sphere", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestParallaxMapping>("Parallax Mapping", SCREEN_WIDTH, SCREEN_HEIGHT);
  gui.registerTest<test::TestCudaMatMul>("CUDA MatMul", SCREEN_WIDTH, SCREEN_HEIGHT);

  // main func
  bool quit = false;
  SDL_Event e;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)) {
        quit = true;
      }
      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) {
        currentTest->OnReload();
      }
      if (!gui.isMouseOverGUI()) currentTest->OnEvent(e);
      ImGui_ImplSDL2_ProcessEvent(&e);  // let imgui interact with SDL
    }

    currentTest->OnRender();
    gui.draw(currentTest->camera);

    SDL_GL_SwapWindow(window);
  }

  gui.shutdown();

  close(window, context);

  return 0;
}
