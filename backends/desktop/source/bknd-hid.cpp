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
std::string* HidGLFW::pText = nullptr;
GLFWcharfun HidGLFW::pOldTextCB = nullptr;
// Default Call back (If no Text input is requsted)
void NullTextCB(GLFWwindow* win, unsigned int c) {
  // Chain
  if (HidGLFW::pOldTextCB) {
    HidGLFW::pOldTextCB(win, c);
  }
}
// Text callback if requested
void TextCB(GLFWwindow* win, unsigned int c) {
  // Chain
  if (HidGLFW::pOldTextCB) {
    HidGLFW::pOldTextCB(win, c);
  }
  if (!HidGLFW::pText) {
    return;
  }
  *HidGLFW::pText += (char)c;
}
HidGLFW::HidGLFW(GLFWwindow* win) : HidDriver("HidGLFW") {
  Window = win;
  HidGLFW::pOldTextCB = glfwSetCharCallback(Window, NullTextCB);
  Flags |= Flags_HasKeyboard;
  Flags |= Flags_HasMouse;
  pBinds[GLFW_MOUSE_BUTTON_LEFT] = Touch;
  /*pBinds[GLFW_KEY_F3] = Kb_3;
  pBinds[GLFW_KEY_ESCAPE] = Kb_Escape;
  pBinds[GLFW_KEY_F11] = Kb_F11;
  pBinds[GLFW_KEY_ESCAPE] = Kb_Escape;
  pBinds[GLFW_KEY_Q] = Kb_Q;
  pBinds[GLFW_KEY_W] = Kb_W;
  pBinds[GLFW_KEY_E] = Kb_E;
  pBinds[GLFW_KEY_R] = Kb_R;
  pBinds[GLFW_KEY_T] = Kb_T;
  pBinds[GLFW_KEY_Z] = Kb_Z;
  pBinds[GLFW_KEY_U] = Kb_U;
  pBinds[GLFW_KEY_I] = Kb_I;
  pBinds[GLFW_KEY_O] = Kb_O;
  pBinds[GLFW_KEY_P] = Kb_P;
  pBinds[GLFW_KEY_A] = Kb_A;
  pBinds[GLFW_KEY_S] = Kb_S;
  pBinds[GLFW_KEY_D] = Kb_D;
  pBinds[GLFW_KEY_F] = Kb_F;
  pBinds[GLFW_KEY_G] = Kb_G;
  pBinds[GLFW_KEY_H] = Kb_H;
  pBinds[GLFW_KEY_J] = Kb_J;
  pBinds[GLFW_KEY_K] = Kb_K;
  pBinds[GLFW_KEY_L] = Kb_L;
  pBinds[GLFW_KEY_Y] = Kb_Y;
  pBinds[GLFW_KEY_X] = Kb_X;
  pBinds[GLFW_KEY_C] = Kb_C;
  pBinds[GLFW_KEY_V] = Kb_V;
  pBinds[GLFW_KEY_B] = Kb_B;
  pBinds[GLFW_KEY_N] = Kb_N;
  pBinds[GLFW_KEY_M] = Kb_M;
  pBinds[GLFW_KEY_LEFT_SHIFT] = Kb_LShift;
  pBinds[GLFW_KEY_F1] = Kb_F1;
  pBinds[GLFW_KEY_F2] = Kb_F2;
  pBinds[GLFW_KEY_F3] = Kb_F3;
  pBinds[GLFW_KEY_F4] = Kb_F4;
  pBinds[GLFW_KEY_F5] = Kb_F5;
  pBinds[GLFW_KEY_F6] = Kb_F6;
  pBinds[GLFW_KEY_F7] = Kb_F7;
  pBinds[GLFW_KEY_F8] = Kb_F8;
  pBinds[GLFW_KEY_F9] = Kb_F9;
  pBinds[GLFW_KEY_F10] = Kb_F10;
  pBinds[GLFW_KEY_F11] = Kb_F11;
  pBinds[GLFW_KEY_F12] = Kb_F12;
  pBinds[GLFW_KEY_1] = Kb_1;
  pBinds[GLFW_KEY_2] = Kb_2;
  pBinds[GLFW_KEY_3] = Kb_3;
  pBinds[GLFW_KEY_4] = Kb_4;
  pBinds[GLFW_KEY_5] = Kb_5;
  pBinds[GLFW_KEY_6] = Kb_6;
  pBinds[GLFW_KEY_7] = Kb_7;
  pBinds[GLFW_KEY_8] = Kb_8;
  pBinds[GLFW_KEY_9] = Kb_9;
  pBinds[GLFW_KEY_0] = Kb_0;
  pBinds[GLFW_KEY_BACKSPACE] = Kb_Backspace;
  pBinds[GLFW_KEY_ENTER] = Kb_Enter;*/
}

void HidGLFW::Update() {
  // Clear States
  for (int i = 0; i < 2; i++) {
    KeyEvents[i][Event_Down] = 0;
    KeyEvents[i][Event_Held] = 0;
    KeyEvents[i][Event_Up] = 0;
    for (auto& it : KbKeyEvents[i]) {
      it.second = Event_Null;
    }
  }
  // Keyboard Logic
  /*for (auto& it : pBinds) {
    int kbstate = glfwGetKey(Window, it.first);
    if (kbstate == GLFW_PRESS) {
      if (PrevStates[it.first] == GLFW_RELEASE) {
        KbKeyEvents[0][it.second] = Event_Down;
      }
      KbKeyEvents[0][it.second] = Event_Held;
    } else if (kbstate == GLFW_RELEASE && PrevStates[it.first] == GLFW_PRESS) {
      KbKeyEvents[0][it.second] = Event_Up;
    }
    PrevStates[it.first] = kbstate;
  }*/
  // Mouse Logic (Todo: Support all mouse buttons)
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
  if (pInTextMode && (PD::OS::GetTime() - pLastUpdate) > 50) {
    pLastUpdate = PD::OS::GetTime();
    HandleTextOps();
  }
}

void HidGLFW::GetInputStr(std::string& str) {
  pText = &str;
  glfwSetCharCallback(Window, TextCB);
  pInTextMode = true;
}

void HidGLFW::HandleTextOps() {
  if (!pText) {
    return;
  }
  /*if (pTimedHeld(Kb_Backspace)) {
    if (!pText->empty()) {
      pText->pop_back();
    }
  } else if (pTimedHeld(Kb_Enter)) {
    *pText += '\n';
  }*/
}
}  // namespace PD