#pragma once

/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

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
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/id.hpp>
#include <pd/ui7/input_api.hpp>
#include <pd/ui7/pd_p_api.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
/**
 * Shared Configuration and Runtime Data for a UI7 Context
 */
class PD_UI7_API IO : public SmartCtor<IO> {
 public:
  /**
   * IO Constructor setting UP References
   */
  IO(Hid::Ref input_driver, LI::Renderer::Ref ren) {
    Time = Timer::New();
    InputHandler = UI7::InputHandler::New(input_driver);
    Theme = UI7::Theme::New();
    Inp = input_driver;
    Ren = ren;
    Back = UI7::DrawList::New(this);
    Front = UI7::DrawList::New(this);
    pRDL = LI::DrawList::New(Ren->WhitePixel);
    DrawListRegestry.PushFront(
        Pair<UI7::ID, DrawList::Ref>("CtxBackList", Back));
    // RegisterDrawList("CtxBackList", Back);
    DeltaStats = TimeStats::New(60);
  };
  ~IO() = default;

  /**
   * IO Update Internal Variables
   */
  void Update();

  float Framerate = 0.f;
  float Delta = 0.f;
  u64 LastTime = 0;
  TimeStats::Ref DeltaStats;
  Timer::Ref Time;
  Hid::Ref Inp;
  LI::Renderer::Ref Ren;
  LI::DrawList::Ref pRDL;
  LI::Font::Ref Font;
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
  PD::List<Pair<UI7::ID, DrawList::Ref>> DrawListRegestry;
  // Short define for DrawKistRegestryLast
  PD::List<Pair<UI7::ID, DrawList::Ref>> pDLRL;
  // std::vector<std::pair<UI7::ID, DrawList::Ref>> DrawListRegestry;
  DrawList::Ref Back;
  DrawList::Ref Front;
  u32 NumVertices = 0;  ///< Debug Vertices Num
  u32 NumIndices = 0;   ///< Debug Indices Num
  Vec<u32> MenuOrder;

  // DrawListApi
  void RegisterDrawList(const UI7::ID& id, DrawList::Ref v) {
    DrawListRegestry.PushBack(Pair(id, v));
  }

  UI7::InputHandler::Ref InputHandler;
};
}  // namespace UI7
}  // namespace PD