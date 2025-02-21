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
#include <pd/core/timetrace.hpp>
#include <pd/lib3ds/drv_hid.hpp>
#include <pd/lithium/renderer.hpp>
#include <pd/overlays/message_mgr.hpp>
#include <pd/overlays/overlay_mgr.hpp>

namespace PD {
/// @brief Template Class for User Application
class App {
 public:
  using AppFlags = u32;
  enum AppFlags_ {
    AppFlags_None = 0,
    AppFLags_UserLoop = 1 << 0,
    AppFlags_HandleOverlays = 1 << 1,
    AppFlags_HandleMessageMgr = 1 << 2,
    AppFlags_HandleRendering = 1 << 3,
    AppFlags_Default = AppFlags_HandleMessageMgr | AppFlags_HandleOverlays |
                       AppFlags_HandleRendering | AppFLags_UserLoop,
  };
  using AppInitFlags = u32;
  enum AppInitFlags_ {
    AppInitFlags_None = 0,
    AppInitFlags_MountRomfs = 1 << 0,
    AppInitFlags_InitGraphics = 1 << 1,
    AppInitFlags_New3dsMode = 1 << 2,
    AppInitFlags_InitGraphicsNoC3D = 1 << 3,
    AppInitFlags_InitLithium = 1 << 4,
    /// I dont have a name for this one yet
    /// It Inits Internal Directory structure
    AppInitFlags_UnnamedOption1 = 1 << 5,
    AppInitFlags_Default = AppInitFlags_MountRomfs | AppInitFlags_InitGraphics |
                           AppInitFlags_New3dsMode | AppInitFlags_InitLithium,
  };
  App(const std::string& name = "App") {
    if (too) {
      Error("Only one App can be created at the same time!");
    }
    this->name = name;
    too++;
  }
  ~App() { too--; }

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

  void FeatureEnable(AppFlags flags) { runtimeflags |= flags; }
  void FeatureDisable(AppFlags flags) { runtimeflags &= ~flags; }
  AppFlags& GetFeatureSet() { return runtimeflags; }

 protected:
  Screen::Ref Top;
  Screen::Ref Bottom;
  AppInitFlags InitFlags = AppInitFlags_Default;

 private:
  AppFlags runtimeflags = AppFlags_Default;
  /// @brief Safe Copy to prevent from editing befor Deinit
  AppInitFlags SafeInitFlags = AppInitFlags_Default;
  void PreInit();
  void PostDeinit();
  LI::Renderer::Ref renderer;
  MessageMgr::Ref msg_mgr;
  OverlayMgr::Ref overlay_mgr;
  Hid::Ref input_mgr;
  u64 last_time;
  float app_time;
  float fps;

  std::string name;

  /// The Only One
  static int too;
};
}  // namespace PD