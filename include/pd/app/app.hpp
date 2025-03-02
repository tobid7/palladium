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
#include <pd/core/timetrace.hpp>
#include <pd/lib3ds/drv_hid.hpp>
#include <pd/lithium/renderer.hpp>
#include <pd/overlays/message_mgr.hpp>
#include <pd/overlays/overlay_mgr.hpp>

namespace PD {
/**
 * Template Class for a User Application on the 3ds
 */
class App {
 public:
  /**
   * alias for AppFlags
   */
  using AppFlags = u32;
  /**
   * App Flags
   *
   * - Probably only the default Setup should be used
   */
  enum AppFlags_ {
    AppFlags_None = 0,                   ///< Do Nothing
    AppFLags_UserLoop = 1 << 0,          ///< Handle User MainLoop
    AppFlags_HandleOverlays = 1 << 1,    ///< Process Overlays
    AppFlags_HandleMessageMgr = 1 << 2,  ///< Process Messages
    AppFlags_HandleRendering = 1 << 3,   ///< Handle Rendering
    /// Default Flags
    AppFlags_Default = AppFlags_HandleMessageMgr | AppFlags_HandleOverlays |
                       AppFlags_HandleRendering | AppFLags_UserLoop,
  };
  /**
   * alias for AppInitFlags
   */
  using AppInitFlags = u32;
  /**
   * App Init Flags
   */
  enum AppInitFlags_ {
    AppInitFlags_None = 0,  ///< Do nothing (probably a useles ability)
    AppInitFlags_MountRomfs = 1 << 0,    ///< Mount Romfs on PreInit
    AppInitFlags_InitGraphics = 1 << 1,  ///< Default Init Graphics for GPU use
    AppInitFlags_New3dsMode = 1 << 2,    ///< Enable New3DS Speedup
    AppInitFlags_InitGraphicsNoC3D = 1 << 3,  ///< Init GFX for Buf Modification
    AppInitFlags_InitLithium = 1 << 4,        ///< Init 2D Rendering Engine
    /// I dont have a name for this one yet
    /// It Inits Internal Directory structure
    AppInitFlags_UnnamedOption1 = 1 << 5,
    AppInitFlags_InitHwInfo = 1 << 6,  ///< Init HwInfo from lib3ds
    /// Default App Init Flags
    AppInitFlags_Default = AppInitFlags_MountRomfs | AppInitFlags_InitGraphics |
                           AppInitFlags_New3dsMode | AppInitFlags_InitLithium,
  };
  /**
   * App Constructor that can Optionally set a name for the App
   * @param name App Name Defaults to App
   */
  App(const std::string& name = "App") {
    if (too) {
      Error("Only one App can be created at the same time!");
    }
    this->name = name;
    too++;
  }
  /**
   * App Deconstructor
   */
  ~App() { too--; }

  /**
   * Templete function where the user can Init his stuff
   */
  virtual void Init() {}
  /**
   * Template function to deinit stuff (most is deinit automatically
   * but it is still possible that some things need to be ordered manually)
   */
  virtual void Deinit() {}
  /**
   * Template User MainLoop
   * @param delta Delta time
   * @param time App Run time
   * @return false to exit the App
   */
  virtual bool MainLoop(float delta, float time) { return false; }

  /**
   * Function to actually run the app
   *
   * Example:
   * ```cpp
   * int main() {
   *    UserApp app;
   *    app.Run();
   *    return 0;
   * }
   * ```
   */
  void Run();
  /**
   * Get the Renderer Reference
   * @return Renderer Reference
   */
  LI::Renderer::Ref Renderer() { return renderer; }
  /**
   * Get Message Manager Reference
   * @return Message Manager Reference
   */
  MessageMgr::Ref Messages() { return msg_mgr; }
  /**
   * Get Overlay Manager Reference
   * @return Overlay Manager Reference
   */
  OverlayMgr::Ref Overlays() { return overlay_mgr; }
  /**
   * Get Input Driver Reference
   * @return Input Driver Reference
   */
  Hid::Ref Input() { return input_mgr; }
  /**
   *  Get Framerate
   * @return frames per second
   */
  float GetFps() const { return fps; }

  /**
   * Enable Runtime Feature(s) (AppFlags)
   * @param flags Flag(s) to enable
   */
  void FeatureEnable(AppFlags flags) { runtimeflags |= flags; }
  /**
   * Disable Runtime Feature(s) (AppFlags)
   * @param flags Flag(s) to disable
   */
  void FeatureDisable(AppFlags flags) { runtimeflags &= ~flags; }
  /**
   * Get Reference Access to runtimeflags
   * @return reference to runtimeflags
   */
  AppFlags& GetFeatureSet() { return runtimeflags; }

  /**
   * Get App Datadirectory (if enabled in AppInitFlags)
   * @return App Data Directory
   */
  std::string GetDataDirectory();

 protected:
  /**
   * Top Screen Reference
   */
  Screen::Ref Top;
  /**
   * Bottom Screen Reference
   */
  Screen::Ref Bottom;
  /**
   * AppInitFlags
   *
   * - Can only be edited in your App class Constructor
   * - Editing them Later will not effect the Deinit Process
   * Example:
   * ```cpp
   * class YourApp : public PD::App {
   *    public:
   *    YourApp(): App("YourApp") {
   *        AppInitFlags |= AppInitFlags_InitLithium;
   *    }
   * }
   * ```
   */
  AppInitFlags InitFlags = AppInitFlags_Default;

 private:
  /** Runtime Flags (can be edited) */
  AppFlags runtimeflags = AppFlags_Default;
  /** Safe Copy to prevent from editing befor Deinit */
  AppInitFlags SafeInitFlags = AppInitFlags_Default;
  /** PreInit Handler */
  void PreInit();
  /** Post Deinit Handler */
  void PostDeinit();
  /** Renderer Reference */
  LI::Renderer::Ref renderer;
  /** Message Manager */
  MessageMgr::Ref msg_mgr;
  /** Overlay Manager */
  OverlayMgr::Ref overlay_mgr;
  /** Input Driver */
  Hid::Ref input_mgr;
  /** Timer to track the App Runtime */
  Timer::Ref app_time;
  /** Last Time (for delta time and fps calculation) */
  u64 last_time;
  /** Framerate */
  float fps;

  /** App Name */
  std::string name;

  /** A static variable to make sure only one App instance can exist */
  static int too;
};
}  // namespace PD