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

#include <pd/core/common.hpp>
#include <pd/core/timer.hpp>
#include <pd/drivers/hid.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/id.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
/**
 * Shared Configuration and Runtime Data for a UI7 Context
 */
class IO : public SmartCtor<IO> {
 public:
  /**
   * IO Constructor setting UP References
   */
  IO(Hid::Ref input_driver, LI::Renderer::Ref ren) {
    Time = Timer::New();
    DragTime = Timer::New(false);
    Theme = UI7::Theme::New();
    Inp = input_driver;
    Ren = ren;
    Back = UI7::DrawList::New(Ren);
    Front = UI7::DrawList::New(Ren);
    RegisterDrawList("CtxBackList", Back);
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
  UI7::Theme::Ref Theme;
  vec2 MenuPadding = 5.f;
  vec2 FramePadding = 5.f;
  vec2 ItemSpace = vec2(5.f, 2.f);
  vec2 MinSliderDragSize = 10.f;  // Min height (Vt) and Min Width (Hz)
  bool ShowMenuBorder = true;
  bool ShowFrameBorder = false; // not implemented yet
  float OverScrollMod = 0.15f;
  u64 DoubleClickTime = 500;  // Milliseconds
  std::vector<std::pair<UI7::ID, DrawList::Ref>> DrawListRegestry;
  DrawList::Ref Back;
  DrawList::Ref Front;
  u32 NumVertices = 0;  ///< Debug Vertices Num
  u32 NumIndices = 0;   ///< Debug Indices Num

  // DrawListApi
  void RegisterDrawList(const UI7::ID& id, DrawList::Ref v) {
    DrawListRegestry.push_back(std::make_pair(id, v));
  }

  // Input API
  u32 FocusedMenu = 0;
  vec4 FocusedMenuRect;
  u32 CurrentMenu = 0;
  u32 DraggedObject = 0;
  vec2 DragSourcePos = 0;
  vec2 DragPosition = 0;
  vec2 DragLastPosition = 0;
  vec4 DragDestination = 0;
  Timer::Ref DragTime;
  u64 DragLastReleased = 0;
  bool DragReleased = false;       ///< Drag Releaded in Box
  bool DragReleasedAW = false;     ///< Drag Released Anywhere
  bool DragDoubleRelease = false;  ///< Double Click
  /** Check if an object is Dragged already */
  bool IsObjectDragged() const { return DraggedObject != 0; }
  /**
   * Function to Check if current Object is dragged
   * or set it dragged
   * @param id ID to identify this specific Object
   * @param area Area where to start dragging
   * @return if inputs to this objects are alowed or not
   */
  bool DragObject(const UI7::ID& id, vec4 area) {
    if (CurrentMenu != FocusedMenu) {
      return false;
    }
    if (IsObjectDragged()) {
      // Only block if the Dragged Object has a difrent id
      if (DraggedObject != id) {
        return false;
      }
    }
    // Get a Short define for touch pos
    vec2 p = Inp->TouchPos();
    // Check if Drag starts in the area position
    if (Inp->IsDown(Inp->Touch) && Ren->InBox(p, area)) {
      // Set ID and iniatial Positions
      DraggedObject = id;
      DragSourcePos = p;
      DragPosition = p;
      DragLastPosition = p;
      DragDestination = area;
      // Reset and Start DragTimer
      DragTime->Reset();
      DragTime->Rseume();
      return false;  // To make sure the Object is "Dragged"
    } else if (Inp->IsHeld(Inp->Touch) && IsObjectDragged()) {
      // Update DragLast and DragPoisition
      DragLastPosition = DragPosition;
      DragPosition = p;
    } else if (Inp->IsUp(Inp->Touch) && IsObjectDragged()) {
      // Released... Everything gets reset
      DraggedObject = 0;
      DragPosition = 0;
      DragSourcePos = 0;
      DragLastPosition = 0;
      DragDestination = 0;
      // Set Drag released to true (only one frame)
      // and Only if still in Box
      DragReleased = Ren->InBox(Inp->TouchPosLast(), area);
      DragReleasedAW = true;  // Advanced
      u64 d_rel = Sys::GetTime();
      if (d_rel - DragLastReleased < DoubleClickTime) {
        DragDoubleRelease = true;
        DragLastReleased = 0;  // Set 0 to prevent double exec
      } else {
        DragLastReleased = d_rel;
      }
      // Ensure timer is paused
      DragTime->Pause();
      DragTime->Reset();
      // Still return The Object is Dragged to ensure
      // the DragReleased var can be used
      return true;
    }
    return IsObjectDragged();
  }
};
}  // namespace UI7
}  // namespace PD