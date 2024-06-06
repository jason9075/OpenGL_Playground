#include "GUI.h"

GUI::GUI(SDL_Window *window, SDL_GLContext context) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init("#version 130");
}

void GUI::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("FPS");
  ImGui::SetWindowPos(ImVec2(0, 0));
  ImGui::Text("%.1f (%.3f ms/f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}
