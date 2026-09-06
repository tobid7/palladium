#include <os/horizon-nx.hpp>

#if defined(__SWITCH__)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <switch.h>

#include <pdsystem>

namespace PD {
struct HorizonNX::Impl {
  GLFWwindow* win = nullptr;
};

void HorizonNX::Init() {
  if (impl) return;
  impl = new Impl();
  socketInitializeDefault();
  nxlinkStdio();
  romfsInit();
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  impl->win =
      glfwCreateWindow(1280, 720, "gfx_test (OpenGL3)", nullptr, nullptr);
  glfwMakeContextCurrent(impl->win);
  gladLoadGL();
  glfwSwapInterval(1);
  PD::Gfx::UseDriver<PD::GfxOpenGL3>();
}

void HorizonNX::Deinit() {
  glfwTerminate();
  if (impl) {
    delete impl;
    impl = nullptr;
  }
}

bool HorizonNX::Mainloop() {
  glfwPollEvents();
  GLFWgamepadstate _gs;
  if (glfwGetGamepadState(GLFW_JOYSTICK_1, &_gs)) {
    if (_gs.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS) {
      glfwSetWindowShouldClose(impl->win, GLFW_TRUE);
    }
  }
  glfwGetFramebufferSize(impl->win, &pViewPort.x, &pViewPort.y);
  return !glfwWindowShouldClose(impl->win);
}

void HorizonNX::ClearViewPort() {
  PD::Gfx::SetViewPort(pViewPort);
  glClearColor(0.1, 0.1, 0.1, 0.1);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, pViewPort.x, pViewPort.y);
}

void HorizonNX::SwapBuffers() { glfwSwapBuffers(impl->win); }

}  // namespace PD
#endif