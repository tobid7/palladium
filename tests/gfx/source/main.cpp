#if defined(__3DS__)
#include <3ds.h>
#include <citro3d.h>
const u32 DisplayTransferFlags =
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
     GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
     GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));
#elif defined(__SWITCH__)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>
#include <switch.h>

static void errorCallback(int errorCode, const char* description) {
  printf("Glfw Error %d: %s\n", errorCode, description);
}

#define PASSERT(x)                             \
  if (!x) {                                    \
    printf("%s: %s", __PRETTY_FUNCTION__, #x); \
    exit(0);                                   \
  }
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
#if defined(__3DS__)
    romfsInit();
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, nullptr);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    Top =
        C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    Bottom =
        C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(Top, GFX_TOP, GFX_LEFT, DisplayTransferFlags);
    C3D_RenderTargetSetOutput(Bottom, GFX_BOTTOM, GFX_LEFT,
                              DisplayTransferFlags);
    PD::Gfx::UseDriver<PD::GfxCitro3D>();
#elif defined(__SWITCH__)
    PASSERT(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window =
        glfwCreateWindow(1280, 720, "gfx_test (OpenGL3)", nullptr, nullptr);
    PASSERT(window);
    glfwMakeContextCurrent(window);
    PASSERT(gladLoadGL());
    glfwSwapInterval(1);
    PD::Gfx::UseDriver<PD::GfxOpenGL3>();
#else
    glfwInit();
    std::string winname = "gfx_test";
    if (d == Driver::OpenGL2) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef __APPLE__
      glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, 0);
#endif
      PD::Gfx::UseDriver<PD::GfxOpenGL2>();
      winname += " (OpenGL2)";
    } else if (d == Driver::OpenGL3) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
      glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, 0);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif
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
#endif
    PD::Gfx::Init();
#if defined(__3DS__) || defined(__SWITCH__)
    pTex = LoadTex("romfs:/icon.png");
#else
    pTex = LoadTex("icon.png");
#endif
    pList.DrawRectFilled(0, 50, 0xff00ffff);
    pList.BindTexture(pTex);
    pList.DrawRectFilled(50, pTex.GetSize(), 0xffffffff);
    pList.DrawCircleFilled(500, 100, 0xffffffff, 50);
    // pList.PathRect(300, 700, 40.f);
    //  pList.PathFill(0xffffffff);
    std::cout << "GfxDriver: " << PD::Gfx::GetDriverName() << std::endl;
  }
  ~App() {
    PD::Gfx::DeleteTexture(pTex);
    PD::Gfx::Deinit();
#if !defined(__3DS__)
    glfwTerminate();
#endif
  }

  void Run() {
#if defined(__3DS__)
    while (aptMainLoop()) {
      PD::Gfx::SetViewPort(400, 240);
      C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
      C3D_FrameDrawOn(Top);
      C3D_RenderTargetClear(Top, C3D_CLEAR_ALL, PD::Color(25, 25, 25, 25), 0);
#elif defined(__SWITCH__)
    while (!glfwWindowShouldClose(window)) {
      GLFWgamepadstate _gs;
      if (glfwGetGamepadState(GLFW_JOYSTICK_1, &_gs)) {
        if (_gs.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS) {
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
      }
      PD::Gfx::SetViewPort(1280, 720);
      glClearColor(0.1, 0.1, 0.1, 0.1);
      glClear(GL_COLOR_BUFFER_BIT);
      glViewport(0, 0, 1280, 720);
#else
    while (!glfwWindowShouldClose(window)) {
      int ww, wh;
      glfwGetFramebufferSize(window, &ww, &wh);
      PD::Gfx::SetViewPort(ww, wh);
      if (pDriver == Driver::OpenGL2 || pDriver == Driver::OpenGL3) {
        glClearColor(0.1, 0.1, 0.1, 0.1);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, ww, wh);
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

      PD::Gfx::Draw(pList);
#if defined(__3DS__)
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
#if defined(__3DS__)
  C3D_RenderTarget* Top = nullptr;
  C3D_RenderTarget* Bottom = nullptr;
#else
  GLFWwindow* window = nullptr;
#endif
  PD::Li::Drawlist pList;
  PD::Li::Texture pTex;
  Driver pDriver;
#ifdef _WIN32
  IDirect3D9* d3d = nullptr;
  IDirect3DDevice9* dx9_device = nullptr;
#endif
};

int main(int argc, char** argv) {
#if defined(__SWITCH__)
  socketInitializeDefault();
  nxlinkStdio();
  romfsInit();
  printf("Starting Palladium GFX Tests...\n");
  glfwSetErrorCallback(errorCallback);
#endif
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
  App* app = new App(drv);
  app->Run();
  delete app;
  return 0;
}