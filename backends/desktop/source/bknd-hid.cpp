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

#include <pd-desktop/bknd-hid.hpp>

namespace PD {
HidGLFW::HidGLFW(GLFWwindow* win) : HidDriver("HidGLFW") {
  Window = win;
  pBinds[GLFW_MOUSE_BUTTON_LEFT] = Touch;
}

void HidGLFW::Update() {
  for (int i = 0; i < 2; i++) {
    KeyEvents[i][Event_Down] = 0;
    KeyEvents[i][Event_Held] = 0;
    KeyEvents[i][Event_Up] = 0;
  }
  int state = glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT);
  if (state == GLFW_PRESS) {
    if (PrevState == GLFW_RELEASE) {
      KeyEvents[0][Event_Down] |= Touch;
    }
    KeyEvents[0][Event_Held] |= Touch;
  } else if (state == GLFW_RELEASE && PrevState == GLFW_PRESS) {
    KeyEvents[0][Event_Up] |= Touch;
  }

  PrevState = state;
  if (pLocked) {
    SwapTab();
  }
  double x, y;
  glfwGetCursorPos(Window, &x, &y);
  pMouse[1] = pMouse[0];  // Cycle pMouse pos
  pMouse[0] = fvec2(x, y);
}
}  // namespace PD