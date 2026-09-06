#include <os/desktopos.hpp>

#if !defined(__SWITCH__) && !defined(__3DS__)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#ifdef _WIN32
#include <d3d9.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <pdsystem>

namespace PD {
struct DesktopOS::Impl {
  GLFWwindow* win = nullptr;
#if WIN32
  IDirect3D9* d3d = nullptr;
  IDirect3DDevice9* dx9_device = nullptr;
#endif
};

void DesktopOS::Init() {
  if (impl) return;
  impl = new Impl();
  std::string winname = "gfx_test";
  glfwInit();
  if (pDriver == Driver::OpenGL2) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    PD::Gfx::UseDriver<PD::GfxOpenGL2>();
    winname += " (OpenGL2)";
  } else if (pDriver == Driver::OpenGL3) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif
    PD::Gfx::UseDriver<PD::GfxOpenGL3>();
    winname += " (OpenGL3)";
  } else if (pDriver == Driver::DirectX9) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    winname += " (DirectX9)";
  }
#ifdef __APPLE__
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, 0);
#endif
  impl->win = glfwCreateWindow(1280, 720, winname.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(impl->win);
  if (pDriver == Driver::OpenGL2 || pDriver == Driver::OpenGL3) {
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
  }
#ifdef _WIN32
  if (pDriver == Driver::DirectX9) {
    impl->d3d = Direct3DCreate9(D3D_SDK_VERSION);
    auto hwnd = glfwGetWin32Window(impl->win);
    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hwnd;

    HRESULT hr = impl->d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                   D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp,
                                   &impl->dx9_device);
    if (FAILED(hr)) {
      MessageBoxW(nullptr, L"Failed to create D3D9 device", L"Error", MB_OK);
      std::abort();
    }
    PD::Gfx::UseDriver<PD::GfxDirectX9>(impl->dx9_device);
  }
#endif
  glfwSwapInterval(1);
}

void DesktopOS::Deinit() {
  glfwTerminate();
  if (impl) {
    delete impl;
    impl = nullptr;
  }
}

bool DesktopOS::Mainloop() {
  glfwPollEvents();
  glfwGetFramebufferSize(impl->win, &pViewPort.x, &pViewPort.y);
  return !glfwWindowShouldClose(impl->win);
}

void DesktopOS::ClearViewPort() {
  PD::Gfx::SetViewPort(pViewPort);
  glClearColor(0.1, 0.1, 0.1, 0.1);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, pViewPort.x, pViewPort.y);
}

void DesktopOS::SwapBuffers() { glfwSwapBuffers(impl->win); }

}  // namespace PD
#endif