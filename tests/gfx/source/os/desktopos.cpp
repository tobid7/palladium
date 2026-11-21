#include <os/desktopos.hpp>
#include <os/metal-layer.hpp>
#include <pdsystem>

#if !defined(__SWITCH__) && !defined(__3DS__)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#ifdef _WIN32
#include <d3d9.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

namespace PD {
struct DesktopOS::Impl {
  GLFWwindow* win = nullptr;
  PD::ivec2 fb;
#if WIN32
  IDirect3D9* d3d = nullptr;
  IDirect3DDevice9* dx9_device = nullptr;
  D3DPRESENT_PARAMETERS d3dpp = {};
#endif
  PD::MetalLayer metal;
  PD::GfxMetalCtx metal_ctx;
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
  } else if (pDriver == Driver::Metal) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    winname += " (Metal)";
  }
#ifdef __APPLE__
  // Full resolution framebuffer on Retina (only affects OpenGL, Metal
  // always gets the backing size via glfwGetFramebufferSize)
  glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
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

    impl->d3dpp = D3DPRESENT_PARAMETERS{};
    impl->d3dpp.Windowed = TRUE;
    impl->d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    impl->d3dpp.hDeviceWindow = hwnd;
    impl->d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    impl->d3dpp.EnableAutoDepthStencil = TRUE;
    impl->d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    HRESULT hr = impl->d3d->CreateDevice(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &impl->d3dpp, &impl->dx9_device);
    if (FAILED(hr)) {
      MessageBoxW(nullptr, L"Failed to create D3D9 device", L"Error", MB_OK);
      std::abort();
    }
    PD::Gfx::UseDriver<PD::GfxDirectX9>(impl->dx9_device);
  }
#endif
#if defined(__APPLE__) && defined(PD_ENABLE_METAL)
  if (pDriver == Driver::Metal) {
    if (!impl->metal.Create(impl->win, impl->metal_ctx)) {
      std::abort();
    }
    PD::Gfx::UseDriver<PD::GfxMetal>(&impl->metal_ctx);
  }
#endif
  glfwSwapInterval(1);
  PD::Hid::UseDriver<HidGlfw>(impl->win);
}

void DesktopOS::Deinit() {
  if (impl) impl->metal.Destroy(impl->metal_ctx);
  glfwTerminate();
  if (impl) {
    delete impl;
    impl = nullptr;
  }
}

bool DesktopOS::Mainloop() {
  glfwPollEvents();
  glfwGetWindowSize(impl->win, &pViewPort.x, &pViewPort.y);
  glfwGetFramebufferSize(impl->win, &impl->fb.x, &impl->fb.y);
  return !glfwWindowShouldClose(impl->win);
}

void DesktopOS::ClearViewPort() {
  PD::Gfx::SetViewPort(pViewPort);
  if (pViewPort.x > 0 && pViewPort.y > 0) {
    PD::Gfx::SetFramebufferScale(
        PD::fvec2(static_cast<float>(impl->fb.x) / pViewPort.x,
                  static_cast<float>(impl->fb.y) / pViewPort.y));
  }
  if (pDriver == Driver::OpenGL3 || pDriver == Driver::OpenGL2) {
    glClearColor(0.1, 0.1, 0.1, 0.1);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, impl->fb.x, impl->fb.y);
  } else if (pDriver == Driver::DirectX9) {
#ifdef _WIN32
    // Resize the swapchain to match the window size
    if (impl->dx9_device && impl->fb.x > 0 && impl->fb.y > 0) {
      if (impl->d3dpp.BackBufferWidth != (UINT)impl->fb.x ||
          impl->d3dpp.BackBufferHeight != (UINT)impl->fb.y) {
        impl->d3dpp.BackBufferWidth = impl->fb.x;
        impl->d3dpp.BackBufferHeight = impl->fb.y;
        impl->dx9_device->Reset(&impl->d3dpp);
      }

      impl->dx9_device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                              D3DCOLOR_XRGB(25, 25, 25), 1.0f, 0);
      impl->dx9_device->BeginScene();
    }
#endif
  } else if (pDriver == Driver::Metal) {
    impl->metal.BeginFrame(impl->metal_ctx, impl->fb);
  }
}

void DesktopOS::SwapBuffers() {
  if (pDriver == Driver::DirectX9) {
#ifdef _WIN32
    // yes we schould use some safetey checks
    if (impl->dx9_device && pViewPort.x > 0 && pViewPort.y > 0) {
      impl->dx9_device->EndScene();
      impl->dx9_device->Present(nullptr, nullptr, nullptr, nullptr);
    }
#endif
  } else if (pDriver == Driver::Metal) {
    impl->metal.EndFrame(impl->metal_ctx);
  } else {
    glfwSwapBuffers(impl->win);
  }
}

}  // namespace PD
#endif