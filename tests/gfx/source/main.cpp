#ifdef __3DS__
#include <3ds.h>
#include <citro3d.h>
const u32 DisplayTransferFlags =
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
     GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
     GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));
#else
#include <glad/glad.h>
//////////////////////////
#include <GLFW/glfw3.h>
#endif

#include <format>
#include <iostream>
#include <palladium>
#include <pdsystem>

#ifdef _WIN32
#include <d3d9.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

enum class Driver {
  Unknown = 0,
  OpenGL2 = 1,
  OpenGL3 = 2,
  DirectX9 = 3,
};

PD::Li::Texture LoadTex(const std::string& path) {
  int w, h, c;
  stbi_uc* buf = stbi_load(path.c_str(), &w, &h, &c, 4);
  return PD::Gfx::LoadTexture(std::vector<PD::u8>(buf, buf + (w * h * 4)), w,
                              h);
}

class App {
 public:
  App(Driver d = Driver::OpenGL3) : pDriver(d) {
    PD::Os::UseDriver<PD::OsDriver>();
#ifndef __3DS__
    glfwInit();
    std::string winname = "gfx_test";
    if (d == Driver::OpenGL2) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
      PD::Gfx::UseDriver<PD::GfxOpenGL2>();
      winname += " (OpenGL2)";
    } else if (d == Driver::OpenGL3) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      PD::Gfx::UseDriver<PD::GfxOpenGL3>();
      winname += " (OpenGL3)";
    } else if (d == Driver::DirectX9) {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      winname += " (DirectX9)";
    }
    window = glfwCreateWindow(1280, 720, winname.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (d == Driver::OpenGL2 || d == Driver::OpenGL3) {
      gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    }
#ifdef _WIN32
    if (d == Driver::DirectX9) {
      d3d = Direct3DCreate9(D3D_SDK_VERSION);
      auto hwnd = glfwGetWin32Window(window);
      D3DPRESENT_PARAMETERS d3dpp = {};
      d3dpp.Windowed = TRUE;
      d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
      d3dpp.hDeviceWindow = hwnd;

      HRESULT hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                     D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                     &d3dpp, &dx9_device);
      if (FAILED(hr)) {
        MessageBoxW(nullptr, L"Failed to create D3D9 device", L"Error", MB_OK);
        std::abort();
      }
      PD::Gfx::UseDriver<PD::GfxDirectX9>(dx9_device);
    }
#endif
    glfwSwapInterval(1);
#else
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    Top =
        C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    Bottom =
        C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(Top, GFX_TOP, GFX_LEFT, DisplayTransferFlags);
    C3D_RenderTargetSetOutput(Bottom, GFX_BOTTOM, GFX_LEFT,
                              DisplayTransferFlags);
    PD::Gfx::UseDriver<PD::GfxCitro3D>();
#endif
    PD::Gfx::Init();
    PD::Li::InitPools(8192);
#ifdef __3DS__
    pTex = LoadTex("sdmc:/icon.png");
#else
    pTex = LoadTex("icon.png");
#endif
    /*std::vector<PD::u8> img(16 * 16 * 4, 0xff);
    pTex = PD::Gfx::LoadTexture(img, 16, 16);*/
    std::cout << "GfxDriver: " << PD::Gfx::GetDriverName() << std::endl;
    pPool.Init(10);
    auto cmd = pPool.Allocate(1);
    cmd->Reserve(4, 6);
    cmd->Add(0, 1, 2);
    cmd->Add(0, 2, 3);
    cmd->Add(
        PD::Li::Vertex(PD::fvec2(0, 0), pTex.GetUV().TopLeft(), 0xffffffff));
    cmd->Add(PD::Li::Vertex(PD::fvec2(pTex.GetSize().x, 0),
                            pTex.GetUV().TopRight(), 0xffffffff));
    cmd->Add(PD::Li::Vertex(PD::fvec2(pTex.GetSize().x, pTex.GetSize().y),
                            pTex.GetUV().BotRight(), 0xffffffff));
    cmd->Add(PD::Li::Vertex(PD::fvec2(0, pTex.GetSize().y),
                            pTex.GetUV().BotLeft(), 0xffffffff));
    cmd->Tex = pTex.GetID();
  }
  ~App() {
    PD::Gfx::DeleteTexture(pTex);
    PD::Gfx::Deinit();
#ifndef __3DS__
    glfwDestroyWindow(window);
    glfwTerminate();
#endif
  }

  void Run() {
#ifdef __3DS__
    while (aptMainLoop()) {
      PD::Gfx::SetViewPort(400, 240);
      C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
      C3D_FrameDrawOn(Top);
      C3D_RenderTargetClear(Top, C3D_CLEAR_ALL, PD::Color(25, 25, 25, 25), 0);
#else
    while (!glfwWindowShouldClose(window)) {
      PD::Gfx::SetViewPort(1280, 720);
      if (pDriver == Driver::OpenGL2 || pDriver == Driver::OpenGL3) {
        glClearColor(0.1, 0.1, 0.1, 0.1);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, 1280, 720);
      } else if (pDriver == Driver::DirectX9) {
#ifdef _WIN32
        if (dx9_device) {
          dx9_device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                            D3DCOLOR_XRGB(25, 25, 25), 1.0f, 0);
          dx9_device->BeginScene();
        }
#endif
      }
#endif
      PD::Gfx::Reset();

      PD::Gfx::Draw(pPool);
#ifdef __3DS__
      C3D_FrameEnd(0);
#else
      glfwPollEvents();
      if (pDriver == Driver::DirectX9) {
#ifdef _WIN32
        if (dx9_device) {
          dx9_device->EndScene();
          dx9_device->Present(nullptr, nullptr, nullptr, nullptr);
        }
#endif
      } else {
        glfwSwapBuffers(window);
      }
#endif
    }
  }

 private:
#ifdef __3DS__
  C3D_RenderTarget* Top = nullptr;
  C3D_RenderTarget* Bottom = nullptr;
#else
  GLFWwindow* window = nullptr;
#endif
  PD::Pool<PD::Li::Command> pPool;
  PD::Li::Texture pTex;
  Driver pDriver;
#ifdef _WIN32
  IDirect3D9* d3d = nullptr;
  IDirect3DDevice9* dx9_device = nullptr;
#endif
};

int main(int argc, char** argv) {
  Driver drv = Driver::OpenGL3;
  if (argc == 2) {
    if (std::string(argv[1]) == "gl2") {
      drv = Driver::OpenGL2;
    } else if (std::string(argv[1]) == "gl3") {
      drv = Driver::OpenGL3;
    } else if (std::string(argv[1]) == "dx9") {
      drv = Driver::DirectX9;
    }
  }
  App app(drv);
  app.Run();
  return 0;
}