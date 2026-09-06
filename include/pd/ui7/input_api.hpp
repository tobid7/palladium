#pragma once

/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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
#include <pd/drivers/drivers.hpp>
#include <pd/lithium/lithium.hpp>
#include <pd/pd_p_api.hpp>
#include <pd/ui7/id.hpp>

namespace PD {
namespace UI7 {
class InputHandler {
 public:
  InputHandler() {}
  ~InputHandler() = default;

  /**
   * Function to Check if current Object is dragged
   * or set it dragged
   * @param id ID to identify this specific Object
   * @param area Area where to start dragging
   * @return if inputs to this objects are alowed or not
   */
  bool DragObject(const UI7::ID& id, fvec4 area) {
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
    fvec2 p = PD::Hid::MousePos();
    // Check if Drag starts in the area position
    if ((PD::Hid::IsEvent(PD::Hid::Event::Down, PD::Hid::Gamepad::Touch) ||
         PD::Hid::IsEvent(PD::Hid::Event::Down,
                          PD::Hid::Keyboard::MouseLeft)) &&
        Li::Math::InBounds(p, area)) {
      // Set ID and iniatial Positions
      DraggedObject = id;
      DragSourcePos = p;
      DragPosition = p;
      DragLastPosition = p;
      DragDestination = area;
      // Reset and Start DragTimer
      DragTime.Reset();
      DragTime.Rseume();
      return false;  // To make sure the Object is "Dragged"
    } else if ((PD::Hid::IsEvent(PD::Hid::Event::Held,
                                 PD::Hid::Gamepad::Touch) ||
                PD::Hid::IsEvent(PD::Hid::Event::Held,
                                 PD::Hid::Keyboard::MouseLeft)) &&
               IsObjectDragged()) {
      // Update DragLast and DragPoisition
      DragLastPosition = DragPosition;
      DragPosition = p;
    } else if ((PD::Hid::IsEvent(PD::Hid::Event::Up, PD::Hid::Gamepad::Touch) ||
                PD::Hid::IsEvent(PD::Hid::Event::Up,
                                 PD::Hid::Keyboard::MouseLeft)) &&
               IsObjectDragged()) {
      // Released... Everything gets reset
      DraggedObject = 0;
      DragPosition = 0;
      DragSourcePos = 0;
      DragLastPosition = 0;
      DragDestination = fvec4(0);
      // Set Drag released to true (only one frame)
      // and Only if still in Box
      DragReleased = Li::Math::InBounds(PD::Hid::MousePosLast(), area);
      DragReleasedAW = true;  // Advanced
      u64 d_rel = PD::Os::GetTime();
      if (d_rel - DragLastReleased < DoubleClickTime) {
        DragDoubleRelease = true;
        DragLastReleased = 0;  // Set 0 to prevent double exec
      } else {
        DragLastReleased = d_rel;
      }
      // Ensure timer is paused
      DragTime.Pause();
      DragTime.Reset();
      // Still return The Object is Dragged to ensure
      // the DragReleased var can be used
      return true;
    }
    return IsObjectDragged();
  }

  void Update() {
    DragTime.Update();
    DragReleased = false;
    DragReleasedAW = false;
    DragDoubleRelease = false;
  }

  u64 DoubleClickTime = 500;  // Milliseconds
  u32 FocusedMenu = 0;
  fvec4 FocusedMenuRect;
  u32 CurrentMenu = 0;
  u32 DraggedObject = 0;
  fvec2 DragSourcePos = 0;
  fvec2 DragPosition = 0;
  fvec2 DragLastPosition = 0;
  /** Fvec4 has an constructor problem currently */
  fvec4 DragDestination = fvec4(0);
  Timer DragTime;
  u64 DragLastReleased = 0;
  bool DragReleased = false;       ///< Drag Releaded in Box
  bool DragReleasedAW = false;     ///< Drag Released Anywhere
  bool DragDoubleRelease = false;  ///< Double Click
  /** Check if an object is Dragged already */
  bool IsObjectDragged() const { return DraggedObject != 0; }
};
}  // namespace UI7
}  // namespace PD