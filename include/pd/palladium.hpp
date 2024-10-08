#pragma once
/// c++ Includes
#include <cstring>
#include <map>
#include <memory>
#include <stack>
#include <string>
/// c includes
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
/// 3ds Includes
#include <3ds.h>
#include <citro3d.h>
/// Palladium Includes
#include <pd/Hardware.hpp>
#include <pd/Overlays.hpp>
#include <pd/Ovl.hpp>
#include <pd/ResultDecoder.hpp>
#include <pd/Sprite.hpp>
#include <pd/Tasks.hpp>
#include <pd/Time.hpp>
#include <pd/base/Color.hpp>
#include <pd/base/FunctionTrace.hpp>
#include <pd/base/Lang.hpp>
#include <pd/base/Memory.hpp>
#include <pd/base/stringtool.hpp>
#include <pd/parameter.hpp>
#include <pd/thread.hpp>

#define PDVSTRING "1.0.0"

#define DEFAULT_CENTER 0.5f

/// @param pd_max_objects Config Param for C2D Mac objects
extern int pd_max_objects;

namespace Palladium {
/// @brief Get Deltatime
/// @return Deltatime
float GetDeltaTime();

/// @brief Scene Class
class Scene {
 public:
  /// @brief Stack of the Scenes
  static std::stack<std::unique_ptr<Scene>> scenes;
  /// @brief Deconstructor
  virtual ~Scene() {}
  virtual void Update() = 0;
  /// @brief Push a Scene to Stack
  /// @param scene Scene to Push
  /// @param fade FadeEffect (Not Correctly Implementet yet)
  static void Load(std::unique_ptr<Scene> scene, bool fade = false);
  /// @brief Go Back a Scene
  static void Back();
  /// @brief do the Draw (Called in Palladium::MainLoop())
  static void doUpdate();
};

/// @brief Integrated Setting Menu of Palladium
class RSettings : public Palladium::Scene {
 private:
  /// @brief State (Define for Menus)
  enum RState {
    RSETTINGS,  // Main Settings Menu
    RIDB,       // Internal Debugger
    ROVERLAYS,  // Overlay Settings
    RFTRACE,    // FTRace Menu
    RUI7,       // UI7 Menu
    RLOGS,      // Logs
    RFV,        // Font Viewer
  };

  /// @param shared_request Defines requests from Draw to Logic
  /// As it is not planned to make Draw non const you'll need
  /// A map of data or bool values that are mutable ake
  /// editable by const functions
  mutable std::map<unsigned int, unsigned int> shared_request;
  /// @param m_state Current menu State (Default=MainMenu aka RSETTINGS)
  Palladium::RSettings::RState m_state =
      Palladium::RSettings::RState::RSETTINGS;

  /// @brief Position in FTrace Menu
  int ftrace_index = 0;

  /// @param mtovlstate State of Metricks Overlay
  std::string mtovlstate = "false";
  /// @param mtscreenstate Screen the Overlay is Set to
  std::string mtscreenstate = "Top";
  std::string kbd_test;
  PDKeyboardState kbd_state;
  bool statemtold = false;
  bool stateftold = false;
  float tmp_txt;

 public:
  /// @brief Constructor
  RSettings();
  /// @brief Deconstructor
  ~RSettings();
  void Update() override;
};

/// @brief Show Up the Palladium-Settings Menu
void LoadSettings();
/// @brief Show Up The Theme Editor
void LoadThemeEditor();
/// @brief Get's The Programs Time running
/// @return Time Running
float GetTime();
/// @brief Get Framerate as Number
/// @return FPS
int GetFps();

/// @brief Get A Rendom Int
/// @param b From
/// @param e To
/// @return Random Int
int GetRandomInt(int b, int e);
/// @brief Fade In
/// @param duration Duration in Frames
void FadeIn();
/// @brief Fade Out
/// @param duration Duration in Frames
void FadeOut();
/// @brief Display Fade Effects
void FadeDisplay();

namespace Init {
/// @brief Init Default Palladium
/// @param app_name Name of Your App
/// @return ResCode
Result Main(std::string app_name = "pdGame");
/// @brief Init Minimal Palladium (For better Hax2.x support)
/// @param app_name Name of Your App
/// @return ResCode
Result Minimal(std::string app_name = "pdGame");
/// @brief Reload the Graphics Engine
/// @return ResCode
Result Reload();
/// @brief Init Graphics Only (NOT SUPPORTET use Reload)
void Graphics();
/// @brief Init Ndsp for Sounds
void NdspFirm();
}  // namespace Init

namespace FS {
/// @brief Check if File exists
/// @param path Path to the File
/// @return exists or not
bool FileExist(const std::string &path);
}  // namespace FS

/// @brief Check if Ndsp is Init
/// @return is or not
bool IsNdspInit();
/// @brief Get Current Framerate as String
/// @return Framerate String
std::string GetFramerate();
/// @brief MainLoop of Palladiums
/// @return Is Still Running or not
bool MainLoop();
/// @brief Exit App (brak the MainLoop)
void ExitApp();

/// @brief Clear the Citro2D TextBuffers
/// @param
void ClearTextBufs(void);

/// @brief Draw Overlays And end the Frame. DO NEVER USE C3D_FRAMEEND cause it
/// breaks Overlay crash Security
void FrameEnd();

/// @brief Returns App Working Directory path
/// @return AppDir Path
std::string GetAppDirectory();
/// @brief returns path to the Data Directory
/// @return data dir path
std::string GetDataDirectory();
}  // namespace Palladium
