/*
MIT License

Copyright (c) 2024 tobid7

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
void App::Run() {
  this->PreInit();
  this->Init();
  last_time = Sys::GetTime();
  while (aptMainLoop()) {
    u64 current = Sys::GetTime();
    u64 dt = current - last_time;
    app_time += float(dt / 1000.f);
    last_time = current;
    fps = 1000.f / (float)dt;
    PD::TT::Beg("App_MainLoop");
    if (!this->MainLoop(dt, app_time)) {
      break;
    }
    PD::TT::End("App_MainLoop");
    renderer->Render();
  }
  this->Deinit();
  this->PostDeinit();
}

void App::PreInit() {
  osSetSpeedupEnable(true);
  gfxInitDefault();
  cfguInit();
  romfsInit();
  renderer = LI::Renderer::New();
}

void App::PostDeinit() {
  renderer = nullptr;
  gfxExit();
  cfguExit();
  romfsExit();
}
}  // namespace PD