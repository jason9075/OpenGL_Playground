#include "GUI.hpp"

#include <OPPCH.h>

GUI::GUI(SDL_Window *window, SDL_GLContext context, test::Test *&currentTest) : currentTest(currentTest) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init("#version 150");
}

//  itemGetter func
bool GUI::itemGetter(void *data, int idx, const char **out_text) {
  const auto &allTests = *static_cast<std::vector<std::pair<std::string, std::function<test::Test *()>>> *>(data);
  if (idx < 0 || idx >= allTests.size()) {
    return false;
  }
  *out_text = allTests[idx].first.c_str();
  return true;
}

void GUI::draw(std::shared_ptr<Camera> camera) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  // start of the info window
  ImGui::Begin("Info");

  // set the window size
  ImGui::SetWindowPos(ImVec2(0, 0));   // top-left corner
  ImGui::SetWindowSize(ImVec2(0, 0));  // auto resize

  ImGui::Text("FPS: %.1f (%.3f ms/f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
  if (ImGui::Combo("##combo", &selectedItem, itemGetter, &allTests, allTests.size())) {
    currentTest->OnExit();
    delete currentTest;
    currentTest = allTests[selectedItem].second();
  }
  if (ImGui::BeginTabBar("TabBar")) {
    if (ImGui::BeginTabItem("param")) {
      currentTest->OnImGuiRender();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("cam")) {
      ImGui::Text("Camera Position:");
      ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->position.x, camera->position.y, camera->position.z);
      ImGui::Text("Camera Orientation:");
      ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->orientation.x, camera->orientation.y, camera->orientation.z);
      ImGui::Text("Look At:");
      ImGui::Text("X:%.2f Y:%.2f Z:%.2f", camera->position.x + camera->orientation.x,
                  camera->position.y + camera->orientation.y, camera->position.z + camera->orientation.z);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("cfg")) {
      if (ImGui::Checkbox("VSync On", &isVSync)) {
        SDL_GL_SetSwapInterval(isVSync);
      }
      if (ImGui::Button("Exit")) {
        currentTest->OnExit();
        delete currentTest;
        exit(0);
      }

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("help")) {
      ImGui::Text("wasd: move");
      ImGui::Text("hjkl: look around");
      ImGui::Text("space/c: up/down");
      ImGui::Text("LMB: look around");
      ImGui::Text("Shift: move faster");
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  // end of the info window
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

bool GUI::isMouseOverGUI() {
  ImGuiIO &io = ImGui::GetIO();
  return io.WantCaptureMouse;
}
