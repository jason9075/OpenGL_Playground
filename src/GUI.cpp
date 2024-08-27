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

void GUI::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Info");
  ImGui::SetWindowPos(ImVec2(0, 0));
  ImGui::Text("FPS: %.1f (%.3f ms/f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
  if (ImGui::Combo("##combo", &selectedItem, itemGetter, &allTests, allTests.size())) {
    currentTest->OnExit();
    delete currentTest;
    currentTest = allTests[selectedItem].second();
  }
  ImVec2 infoWindowPos = ImGui::GetWindowPos();
  ImVec2 infoWindowSize = ImGui::GetWindowSize();
  ImGui::End();

  ImGui::Begin("Settings");
  currentTest->OnImGuiRender();
  ImGui::SetWindowPos(ImVec2(0, infoWindowPos.y + infoWindowSize.y));
  ImGui::SetWindowSize(ImVec2(infoWindowSize.x, 0));
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
