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

#include <GLFW/glfw3.h>

#include <pd/drivers/drivers.hpp>

namespace PD {
class HidGLFW : public HidDriver {
 public:
  HidGLFW(GLFWwindow* win);
  ~HidGLFW() = default;

  PD_SHARED(HidGLFW);

  void Update() override;
  void GetInputStr(std::string& str) override;
  void HandleTextOps();
  bool pTimedHeld(KbKey k) {
    if (pTimings.count(k)) {
      if (IsEvent(Event_Up, k)) {
        pTimings.erase(k);
        return false;
      }
      return (PD::OS::GetTime() - pTimings[k]) > 50;
    }
    if (!IsEvent(Event_Held, k)) {
      if (pTimings.count(k)) {
        pTimings.erase(k);
        return false;
      }
    }
    if (IsEvent(Event_Held, k)) {
      pTimings[k] = PD::OS::GetTime();
      return true;
    }
    return false;
  }

  /** Data section */
  GLFWwindow* Window;
  int PrevState;
  std::unordered_map<int, int> PrevStates;
  static GLFWcharfun pOldTextCB;
  static std::string* pText;
  bool pInTextMode = false;
  PD::u64 pLastUpdate = 0;
  std::unordered_map<KbKey, u64> pTimings;
};
}  // namespace PD