/*
MIT License

Copyright (c) 2024 - 2025 tobid7

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <3ds.h>

#include <pd/common/app.hpp>
#include <pd/common/sys.hpp>

namespace PD {
int App::too;

void App::Run() {
  this->PreInit();
  this->Init();
  last_time = Sys::GetTime();
  while (aptMainLoop()) {
    input_mgr->Update();
    u64 current = Sys::GetNanoTime();
    float dt = static_cast<float>(current - last_time) / 1000000.f;
    app_time += dt / 1000.f;
    last_time = current;
    fps = 1000.f / dt;
    if (runtimeflags & AppFLags_UserLoop) {
      PD::TT::Beg("App_MainLoop");
      if (!this->MainLoop(dt, app_time)) {
        break;
      }
      PD::TT::End("App_MainLoop");
    }
    PD::TT::Beg("Ovl_Update");
    if (runtimeflags & AppFlags_HandleOverlays &&
        SafeInitFlags & AppInitFlags_InitLithium) {
      renderer->Layer(90);
      overlay_mgr->Update(dt);
    }
    if (runtimeflags & AppFlags_HandleMessageMgr &&
        SafeInitFlags & AppInitFlags_InitLithium) {
      /// Messages have their own special Layer
      renderer->Layer(93);
      msg_mgr->Update(dt);
    }
    PD::TT::End("Ovl_Update");
    if (runtimeflags & AppFlags_HandleRendering &&
        SafeInitFlags & AppInitFlags_InitLithium) {
      renderer->PrepareRender();
      renderer->Render(Top);
      renderer->Render(Bottom);
      renderer->FinalizeRender();
    }
  }
  this->Deinit();
  this->PostDeinit();
}

void App::PreInit() {
  /// Create a Copy that won't get edit
  SafeInitFlags = InitFlags;
  osSetSpeedupEnable(InitFlags & AppInitFlags_New3dsMode);
  if (InitFlags & AppInitFlags_InitGraphics) {
    gfxInitDefault();
    if (!(InitFlags & AppInitFlags_InitGraphicsNoC3D)) {
      C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    }
  }
  cfguInit();
  if (InitFlags & AppInitFlags_MountRomfs) {
    romfsInit();
  }
  input_mgr = Hid::New();
  if (InitFlags & AppInitFlags_InitLithium) {
    Assert(!(InitFlags & AppInitFlags_InitGraphicsNoC3D),
           "InitGraphicsNoC3D is not compatible with InitLithium!");
    Top = Screen::New(Screen::Top);
    Bottom = Screen::New(Screen::Bottom);
    renderer = LI::Renderer::New();
    renderer->RegisterScreen(false, Top);
    renderer->RegisterScreen(true, Bottom);
    renderer->OnScreen(Top);
    msg_mgr = MessageMgr::New(renderer);
    overlay_mgr = OverlayMgr::New(renderer, input_mgr);
  }
}

void App::PostDeinit() {
  renderer = nullptr;
  msg_mgr = nullptr;
  overlay_mgr = nullptr;
  input_mgr = nullptr;
  if (SafeInitFlags & AppInitFlags_InitGraphics) {
    if (!(SafeInitFlags & AppInitFlags_InitGraphicsNoC3D)) {
      C3D_Fini();
    }
    gfxExit();
  }
  cfguExit();
  if (SafeInitFlags & AppInitFlags_MountRomfs) {
    romfsExit();
  }
}
}  // namespace PD