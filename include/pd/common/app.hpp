#pragma once

/*
MIT License
Copyright (c) 2024 René Amthor (tobid7)

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

#include <pd/common/common.hpp>
#include <pd/common/timetrace.hpp>
#include <pd/controls/hid.hpp>
#include <pd/graphics/lithium.hpp>
#include <pd/overlays/message_mgr.hpp>
#include <pd/overlays/overlay_mgr.hpp>

namespace PD {
/// @brief Template Class for User Application
class App {
 public:
  App() {
    if (too) {
      Error("Only one App can be created at the same time!");
    }
    too++;
  }
  ~App() = default;

  /// @brief Templete function where the user can Init his stuff
  virtual void Init() {}
  /// @brief Templeta funciton to deinit stuff
  /// (most of that is done automatically)
  virtual void Deinit() {}
  /// @brief App Mainloop
  /// @param delta Deltatime
  /// @param time App RunTime
  /// @return false to exit the app
  virtual bool MainLoop(float delta, float time) { return false; }

  /// @brief Function to run the App
  /// (int main() {
  ///   UserApp app;
  ///   app.Run();
  ///   return 0;
  /// })
  void Run();
  LI::Renderer::Ref Renderer() { return renderer; }
  MessageMgr::Ref Messages() { return msg_mgr; }
  OverlayMgr::Ref Overlays() { return overlay_mgr; }
  Hid::Ref Input() { return input_mgr; }
  float GetFps() const { return fps; }

 protected:
  Screen::Ref Top;
  Screen::Ref Bottom;

 private:
  void PreInit();
  void PostDeinit();
  LI::Renderer::Ref renderer;
  MessageMgr::Ref msg_mgr;
  OverlayMgr::Ref overlay_mgr;
  Hid::Ref input_mgr;
  u64 last_time;
  float app_time;
  float fps;

  /// The Only One
  static int too;
};
}  // namespace PD