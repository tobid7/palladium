#include <pd_hid_glfw.hpp>

namespace PD {
PD_BKND_DESKTOP_API HidGLFW::HidGLFW(GLFWwindow* win) : Hid("GLFW") {
  Window = win;
  binds[GLFW_MOUSE_BUTTON_LEFT] = Touch;
}
PD_BKND_DESKTOP_API void HidGLFW::Update() {
  for (int i = 0; i < 2; i++) {
    key_events[i][Event_Down] = 0;
    key_events[i][Event_Held] = 0;
    key_events[i][Event_Up] = 0;
  }
  int state = glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT);
  if (state == GLFW_PRESS) {
    if (PrevState == GLFW_RELEASE) {
      key_events[0][Event_Down] |= Touch;
    }
    key_events[0][Event_Held] |= Touch;
  } else if (state == GLFW_RELEASE && PrevState == GLFW_PRESS) {
    key_events[0][Event_Up] |= Touch;
  }

  PrevState = state;
  if (locked) {
    SwappyTable();
  }
  double x, y;
  glfwGetCursorPos(Window, &x, &y);
  touch[1] = touch[0];  // Cycle touch pos
  touch[0] = fvec2(x, y);
}
}  // namespace PD