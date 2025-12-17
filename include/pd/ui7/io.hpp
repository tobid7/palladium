#pragma once

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

#include <pd/core/core.hpp>
#include <pd/ui7/input_api.hpp>
#include <pd/ui7/pd_p_api.hpp>
#include <pd/ui7/theme.hpp>
#include <pd/ui7/viewport.hpp>

namespace PD {
namespace UI7 {
class PD_UI7_API IO {
 public:
  IO() {
    Time = Timer::New();
    InputHandler = InputHandler::New();
    Theme = UI7::Theme::New();
    Back = Li::DrawList::New();
    Front = Li::DrawList::New();
    FDL = Li::DrawList::New();
    DeltaStats = TimeStats::New(60);
    /** Probably not the best solution i guess */
    CurrentViewPort.z = PD::Gfx::pGfx->ViewPort.x;
    CurrentViewPort.w = PD::Gfx::pGfx->ViewPort.y;
  }
  ~IO() {}

  PD_SHARED(IO);

  /**
   * IO Update Internal Variables
   */
  void Update();

  /**
   * Final Draw List for PD::Li::Gfx::RednerDrawData
   *
   * Possible thanks to the DrawList::Merge Feature
   */
  Li::DrawList::Ref FDL = nullptr;
  ivec4 CurrentViewPort = ivec4(0, 0, 0, 0);
  std::unordered_map<u32, ViewPort::Ref> ViewPorts;
  float Framerate = 0.f;
  float Delta = 0.f;
  u64 LastTime = 0;
  TimeStats::Ref DeltaStats;
  Timer::Ref Time;
  Li::Font::Ref Font;
  float FontScale = 0.7f;
  UI7::Theme::Ref Theme;
  fvec2 MenuPadding = 5.f;
  fvec2 FramePadding = 5.f;
  fvec2 ItemSpace = vec2(5.f, 2.f);
  fvec2 MinSliderDragSize = 10.f;  // Min height (Vt) and Min Width (Hz)
  bool ShowMenuBorder = true;
  bool ShowFrameBorder = false;  // not implemented yet
  float OverScrollMod = 0.15f;
  u64 DoubleClickTime = 500;  // Milliseconds
  std::list<std::pair<UI7::ID, Li::DrawList::Ref>> DrawListRegestry;
  // Short define for DrawKistRegestryLast
  std::list<std::pair<UI7::ID, Li::DrawList::Ref>> pDLRL;
  Li::DrawList::Ref Back;
  Li::DrawList::Ref Front;
  u32 NumVertices = 0;  ///< Debug Vertices Num
  u32 NumIndices = 0;   ///< Debug Indices Num
  std::vector<u32> MenuOrder;

  // DrawListApi
  void RegisterDrawList(const UI7::ID& id, Li::DrawList::Ref v) {
    DrawListRegestry.push_back(std::make_pair(id, v));
  }

  void AddViewPort(const ID& id, const ivec4& size) {
    if (ViewPorts.count(id)) {
      return;
    }
    ViewPorts[id] = ViewPort::New(id, size);
  }

  ViewPort::Ref GetViewPort(const ID& id) {
    if (!ViewPorts.count(id)) {
      return nullptr;
    }
    return ViewPorts[id];
  }

  UI7::InputHandler::Ref InputHandler;
};
}  // namespace UI7
}  // namespace PD