#include <pd/Hid.hpp>  // Integate HidApi
#include <pd/Lithium.hpp>
#include <pd/Message.hpp>
#include <pd/Overlays.hpp>
#include <pd/ThemeEditor.hpp>
#include <pd/UI7.hpp>
#include <pd/palladium.hpp>

// Config 2
#include <pd/external/json.hpp>
#include <pd/internal_db.hpp>

// C++ includes
#include <filesystem>
#include <fstream>
#include <random>

#define DISPLAY_TRANSFER_FLAGS                                              \
  (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |                    \
   GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
   GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |                           \
   GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

void exit_romfs() { romfsExit(); }

// OVL FLAG ENTRY
void OverlayFlag(PDMetrikOverlayFlags &flags, int flag, std::string name) {
  bool val = flags & flag;
  bool valb = val;
  UI7::Checkbox(name, val);
  if (val != valb) {
    if (val)
      flags |= flag;
    else
      flags &= ~flag;
  }
}

// TODO: Better Fader
void Npifade() {
  if (pdi_fadein) {
    if (pdi_fadealpha < 255) {
      if ((int)pdi_fadealpha + 3 > 255) {
        pdi_fadealpha = 255;
      } else {
        pdi_fadealpha += 3;
      }
    } else {
      pdi_fadein = false;
    }
  } else if (pdi_fadeout) {
    if (pdi_fadealpha > 0) {
      if ((int)pdi_fadealpha - 3 < 0) {
        pdi_fadealpha = 0;
      } else {
        pdi_fadealpha -= 3;
      }
    } else {
      pdi_fadeout = false;
    }
  } else {
    if (pdi_wait_fade) pdi_wait_fade = false;
    if (pdi_fade_exit) pdi_running = false;
    if (pdi_fade_scene_wait) {
      Palladium::Scene::scenes.push(std::move(pdi_fade_scene));
      pdi_fade_scene_wait = false;
      Palladium::FadeIn();
    }
    // No fade
  }
  /*if (pdi_fadein || pdi_fadeout) {
    LI::OnScreen(LIScreen_Top);
    LI::AddRect(NVec2(0, 0), NVec2(400, 240),
                            ((pdi_fadealpha << 24) | 0x00000000));
    LI::OnScreen(LIScreen_Bottom);
    LI::AddRect(NVec2(0, 0), NVec2(320, 240),
                            ((pdi_fadealpha << 24) | 0x00000000));
  }*/
}

void pdi_init_input() {
  Palladium::Hid::RegTouchCoords(pdi_hid_touch_pos);
  Palladium::Hid::RegKeyDown(d7_hDown);
  Palladium::Hid::RegKeyHeld(d7_hHeld);
  Palladium::Hid::RegKeyUp(d7_hUp);
  Palladium::Hid::RegKeyRepeat(d7_hRepeat);
  Palladium::Hid::RegKeyEvent("touch", KEY_TOUCH);
  // Default Buttons
  Palladium::Hid::RegKeyEvent("confirm", KEY_A);
  Palladium::Hid::RegKeyEvent("cancel", KEY_B);
  Palladium::Hid::RegKeyEvent("spec2", KEY_X);
  Palladium::Hid::RegKeyEvent("spec3", KEY_Y);
  Palladium::Hid::RegKeyEvent("options", KEY_SELECT);
  Palladium::Hid::RegKeyEvent("spec1", KEY_START);
  // Dpad only
  Palladium::Hid::RegKeyEvent("dright", KEY_DRIGHT);
  Palladium::Hid::RegKeyEvent("dleft", KEY_DLEFT);
  Palladium::Hid::RegKeyEvent("dup", KEY_DUP);
  Palladium::Hid::RegKeyEvent("ddown", KEY_DDOWN);
  // D + Cpad
  Palladium::Hid::RegKeyEvent("right", KEY_RIGHT);
  Palladium::Hid::RegKeyEvent("left", KEY_LEFT);
  Palladium::Hid::RegKeyEvent("up", KEY_UP);
  Palladium::Hid::RegKeyEvent("down", KEY_DOWN);
  // Back keys
  Palladium::Hid::RegKeyEvent("rt1", KEY_R);
  Palladium::Hid::RegKeyEvent("lt1", KEY_L);
  Palladium::Hid::RegKeyEvent("rt2", KEY_ZL);
  Palladium::Hid::RegKeyEvent("lt2", KEY_ZR);
  // Key by their names
  Palladium::Hid::RegKeyEvent("A", KEY_A);
  Palladium::Hid::RegKeyEvent("B", KEY_B);
  Palladium::Hid::RegKeyEvent("X", KEY_X);
  Palladium::Hid::RegKeyEvent("Y", KEY_Y);
  Palladium::Hid::RegKeyEvent("L", KEY_L);
  Palladium::Hid::RegKeyEvent("R", KEY_R);
  Palladium::Hid::RegKeyEvent("ZR", KEY_ZR);
  Palladium::Hid::RegKeyEvent("ZL", KEY_ZL);
  Palladium::Hid::RegKeyEvent("START", KEY_START);
  Palladium::Hid::RegKeyEvent("SELECT", KEY_SELECT);
  Palladium::Hid::RegKeyEvent("DUP", KEY_DUP);
  Palladium::Hid::RegKeyEvent("DDOWN", KEY_DDOWN);
  Palladium::Hid::RegKeyEvent("DLEFT", KEY_DRIGHT);
  Palladium::Hid::RegKeyEvent("DRIGHT", KEY_DLEFT);
}

void pdi_init_config() {
  pdi_config_path = Palladium::GetAppDirectory();
  std::filesystem::create_directories(pdi_config_path.c_str());
  std::filesystem::create_directories("sdmc:/Palladium/Reports");
  bool renew = false;

  if (Palladium::FS::FileExist(pdi_config_path + "/config.rc7")) {
    std::fstream cfg_ldr(pdi_config_path + "/config.rc7", std::ios::in);
    cfg_ldr >> pdi_config;
    cfg_ldr.close();
    std::string version = pdi_config["info"]["version"].get<std::string>();
    if (version != CFGVER) renew = true;
  }

  if (!Palladium::FS::FileExist(pdi_config_path + "/config.rc7") || renew) {
    if (renew) {
      std::fstream cfg_ldr(pdi_config_path + "/config.rc7", std::ios::in);
      cfg_ldr >> pdi_config;
      cfg_ldr.close();
    }
    pdi_config.clear();
    pdi_config["info"]["version"] = CFGVER;
    pdi_config["info"]["Palladiumver"] = PDVSTRING;
    pdi_config["metrik-settings"]["show"] = false;
    pdi_config["metrik-settings"]["Screen"] = true;
    pdi_config["metrik-settings"]["Text"] = 0xffffffff;
    pdi_config["metrik-settings"]["Bg"] = 0xaa000000;
    pdi_config["metrik-settings"]["Size"] = 0.7f;
    pdi_config["metrik-settings"]["config"] = PDMetrikOverlayFlags_Default;
    std::fstream cfg_wrt(pdi_config_path + "/config.rc7", std::ios::out);
    cfg_wrt << pdi_config.dump(4);
    cfg_wrt.close();
  }
  std::fstream cfg_ldr(pdi_config_path + "/config.rc7", std::ios::in);
  cfg_ldr >> pdi_config;
  cfg_ldr.close();

  pdi_metrikd = pdi_config["metrik-settings"]["show"].get<bool>();
  pdi_mt_txtSize = pdi_config["metrik-settings"]["Size"].get<float>();
  pdi_mt_screen = pdi_config["metrik-settings"]["Screen"].get<bool>();
  pdi_mt_txtcolor = pdi_config["metrik-settings"]["Text"].get<unsigned int>();
  pdi_mt_color = pdi_config["metrik-settings"]["Bg"].get<unsigned int>();
  pd_ovl_flags = pdi_config["metrik-settings"]["config"].get<int>();

  if (pdi_metrikd)
    Palladium::AddOvl(std::make_unique<Palladium::Ovl_Metrik>(
        &pdi_metrikd, &pdi_mt_screen, &pdi_mt_color, &pdi_mt_txtcolor,
        &pdi_mt_txtSize));
}

void pdi_init_theme() {
  if (pdi_config_path == "") {
    pdi_config_path = "sdmc:/Palladium/Apps/";
    pdi_config_path += pdi_app_name;
  }
  std::string path = pdi_config_path + "/themes";
  std::filesystem::create_directories(path.c_str());
  bool renew = false;

  if (Palladium::FS::FileExist(path + "/Palladium.theme")) {
    std::fstream cfg_ldr(path + "/Palladium.theme", std::ios::in);
    nlohmann::json js;
    cfg_ldr >> js;
    cfg_ldr.close();
    std::string version = js["version"].get<std::string>();
    if (version != THEMEVER) renew = true;
  }

  if (!Palladium::FS::FileExist(path + "/Palladium.theme") || renew) {
    pdi_amdt = true;
    Palladium::ThemeActive()->Save(path + "/Palladium.theme");
    pdi_amdt = false;
  }
}

float Palladium::GetDeltaTime() { return pdi_dtm; }

void Palladium::Init::NdspFirm() {
  if (access("sdmc:/3ds/dspfirm.cdc", F_OK) != -1) {
    ndspInit();
    atexit(ndspExit);
    pdi_is_ndsp = true;
  } else {
    Palladium::PushMessage(Palladium::Message(
        "Palladium", "dspfirm.cdc not found!\nUnable to play sounds!"));
  }
}

void Palladium::Scene::doUpdate() {
  Ftrace::ScopedTrace st("pd-core", f2s(Scene::doUpdate));
  if (!Palladium::Scene::scenes.empty())
    Palladium::Scene::scenes.top()->Update();
}

void Palladium::Scene::Load(std::unique_ptr<Scene> scene, bool fade) {
  if (fade) {
    Palladium::FadeOut();
    pdi_fade_scene = std::move(scene);
    pdi_fade_scene_wait = true;
  } else
    Scene::scenes.push(std::move(scene));
}

void Palladium::Scene::Back() {
  if (Palladium::Scene::scenes.size() > 0) Palladium::Scene::scenes.pop();
}

void frameloop() {
  pdi_frames++;
  pdi_delta_time = osGetTime() - pdi_last_time;
  if (pdi_delta_time >= 1000) {
    pdi_framerate = pdi_frames / (pdi_delta_time / 1000.0f) + 1;
    pdi_frames = 0;
    pdi_last_time = osGetTime();
  }
}
float getframerate() { return pdi_framerate; }

std::string Palladium::GetFramerate() {
  return (std::to_string((int)pdi_framerate).substr(0, 2));
}

bool Palladium::MainLoop() {
  if (!aptMainLoop()) return false;
  // Deltatime
  uint64_t currentTime = svcGetSystemTick();
  pdi_dtm = static_cast<float>(currentTime - pdi_last_tm) / TICKS_PER_MSEC;
  pdi_time += pdi_dtm * 0.001f;
  pdi_last_tm = currentTime;

  hidScanInput();
  d7_hDown = hidKeysDown();
  d7_hUp = hidKeysUp();
  d7_hHeld = hidKeysHeld();
  // Inofficial
  d7_hRepeat = hidKeysDownRepeat();
  hidTouchRead(&d7_touch);
  Hid::Update();
  pdi_hid_touch_pos = NVec2(d7_touch.px, d7_touch.py);

  Palladium::Ftrace::End("pd-core", f2s(MainLoop));
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
  Palladium::Ftrace::Beg("pd-core", f2s(MainLoop));

  C3D_RenderTargetClear(pd_top, C3D_CLEAR_ALL, 0x00000000, 0);
  C3D_RenderTargetClear(pd_top_right, C3D_CLEAR_ALL, 0x00000000, 0);
  C3D_RenderTargetClear(pd_bottom, C3D_CLEAR_ALL, 0x00000000, 0);
  frameloop();
  if (pdi_enable_scene_system) {
    Palladium::Scene::doUpdate();
  }
  return pdi_running;
}

void Palladium::Init::Graphics() {
  // C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  // C2D_Init((size_t)pd_max_objects);
  // C2D_Prepare();
  // pd_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  // pd_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  // pd_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
}

Result Palladium::Init::Main(std::string app_name) {
  Palladium::Ftrace::ScopedTrace st("pd-core", f2s(Init::Main));
  pdi_app_name = app_name;

  pdi_enable_scene_system = (pd_flags & PDFlags_SceneSystem);

  gfxInitDefault();
  atexit(gfxExit);
  // Speedup
  osSetSpeedupEnable(true);
  // consoleInit(GFX_TOP, NULL);
  cfguInit();
  atexit(cfguExit);
  CFGU_SecureInfoGetRegion(&pdi_system_region);
  CFGU_GetSystemModel(&pdi_console_model);

  aptInit();
  atexit(aptExit);
  romfsInit();

  pdi_init_config();

  pdi_active_theme = Theme::New();
  pdi_active_theme->Default();

  auto ret = pdi_soc_init();
  if (ret) {
    Palladium::PushMessage("Palladium", "Failed to\nInit Soc!");
  } else {
    atexit(pdi_soc_deinit);
  }

  if (R_SUCCEEDED(amInit())) {
    atexit(amExit);
    pdi_is_am_init = true;
  }

  Hardware::Initialisize();

  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  atexit(C3D_Fini);

  pd_top =
      C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(pd_top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
  pd_top_right =
      C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(pd_top_right, GFX_TOP, GFX_RIGHT,
                            DISPLAY_TRANSFER_FLAGS);
  pd_bottom =
      C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(pd_bottom, GFX_BOTTOM, GFX_LEFT,
                            DISPLAY_TRANSFER_FLAGS);
  LI::Init();
  atexit(LI::Exit);
  atexit(exit_romfs);

  pdi_graphics_on = true;
  pdi_last_tm = svcGetSystemTick();

  pdi_init_input();
  pdi_init_theme();
  UI7::Init();
  atexit(UI7::Deinit);
  pdi_running = true;
  return 0;
}

Result Palladium::Init::Minimal(std::string app_name) {
  Palladium::Ftrace::ScopedTrace st("pd-core", f2s(Init::Minimal));
  pdi_app_name = app_name;

  pdi_enable_scene_system = (pd_flags & PDFlags_SceneSystem);

  gfxInitDefault();
  atexit(gfxExit);
  romfsInit();

  pdi_init_config();

  pdi_active_theme = Theme::New();
  pdi_active_theme->Default();

  auto ret = pdi_soc_init();
  if (ret) {
    Palladium::PushMessage("Palladium", "Failed to\nInit Soc!");
  } else {
    atexit(pdi_soc_deinit);
  }

  if (R_SUCCEEDED(amInit())) {
    atexit(amExit);
    pdi_is_am_init = true;
  }

  Hardware::Initialisize();

  osSetSpeedupEnable(true);
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  atexit(C3D_Fini);

  pd_top =
      C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(pd_top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
  pd_top_right =
      C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(pd_top_right, GFX_TOP, GFX_RIGHT,
                            DISPLAY_TRANSFER_FLAGS);
  pd_bottom =
      C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(pd_bottom, GFX_BOTTOM, GFX_LEFT,
                            DISPLAY_TRANSFER_FLAGS);
  LI::Init();
  atexit(LI::Exit);
  atexit(exit_romfs);

  pdi_graphics_on = true;

  // Check if citra
  s64 citracheck = 0;
  svcGetSystemInfo(&citracheck, 0x20000, 0);
  pdi_is_citra = citracheck ? true : false;

  pdi_init_input();
  pdi_init_theme();
  UI7::Init();
  atexit(UI7::Deinit);
  pdi_running = true;
  return 0;
}

Result Palladium::Init::Reload() {
  // pdi_graphics_on = false;
  // C3D_Fini();
  // C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  // pdi_graphics_on = true;

  return 0;
}

void Palladium::ExitApp() {
  if (pdi_wait_fade) {
    pdi_fade_exit = true;
  } else
    pdi_running = false;
}

int Palladium::GetRandomInt(int b, int e) {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(b, e);
  int r = distribution(generator);
  return r;
}

bool Palladium::FS::FileExist(const std::string &path) {
  return std::filesystem::exists(path) &&
         std::filesystem::is_regular_file(path);
}

int Palladium::GetFps() { return (int)pdi_framerate; }

bool Palladium::IsNdspInit() { return pdi_is_ndsp; }

void OvlHandler() {
  Palladium::Ftrace::ScopedTrace st("pd-core", f2s(OvlHandler));
  for (size_t i = 0; i < pdi_overlays.size(); i++) {
    pdi_overlays[i]->Draw();
    pdi_overlays[i]->Logic();
    if (pdi_overlays[i]->IsKilled())
      pdi_overlays.erase(pdi_overlays.begin() + i);
  }
}

void Palladium::FrameEnd() {
  Ftrace::ScopedTrace st("pd-core", f2s(FrameEnd));
  C3D_FrameBegin(2);
  if (!pdi_enable_scene_system && pdi_settings) {
    Palladium::Scene::doUpdate();
  }
  UI7::Update();
  UI7::Debug();
  // Use Heigh Layer for Overlays
  LI::Layer(LI::Layer() + 100);
  Palladium::ProcessMessages();
  OvlHandler();
  Npifade();
  LI::Render(pd_top, pd_bottom);
  C3D_FrameEnd(0);
}

Palladium::RSettings::RSettings() {
  // Palladium Settings is designed for
  // System Font
  tmp_txt = LI::GetTextScale();
  LI::DefaultTextScale();
  Palladium::FadeIn();
  std::fstream cfg_ldr(pdi_config_path + "/config.rc7", std::ios::in);
  cfg_ldr >> pdi_config;
  cfg_ldr.close();
  pdi_settings = true;
  statemtold = pdi_metrikd;
  stateftold = pdi_ftraced;
}

Palladium::RSettings::~RSettings() { LI::SetTextScale(tmp_txt); }

std::vector<std::string> StrHelper(std::string input) {
  std::string ss(input);
  std::istringstream in(ss);
  std::vector<std::string> test1;

  std::copy(std::istream_iterator<std::string>(in),
            std::istream_iterator<std::string>(), std::back_inserter(test1));
  return test1;
}

void Palladium::RSettings::Update() {
  // Rendering / UI Logic
  /// TODO: Update code for new system
  if (m_state == RSETTINGS) {
    LI::OnScreen(false);
    if (UI7::BeginMenu("Palladium -> Settings")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Config Version: " + std::string(CFGVER));
      UI7::Label("App: " + pdi_app_name);
      UI7::Label("Palladium: " + std::string(PDVSTRING));
      UI7::Label("Citra: " + std::string(pdi_is_citra ? "true" : "false"));
      UI7::Label("Current: " + std::to_string(Palladium::Memory::GetCurrent()) +
                 "b");
      UI7::Label("Delta: " + std::to_string(Palladium::GetDeltaTime()));
      UI7::Label("Time: " + std::to_string(Palladium::GetTime()));
      UI7::Label("Kbd test: " + kbd_test);
      UI7::EndMenu();
    }
    LI::OnScreen(true);
    if (UI7::BeginMenu("Press B to go back!")) {
      if (UI7::Button("FTrace")) {
        shared_request[0x00000001] = RFTRACE;
      }
      if (UI7::Button("UI7")) {
        shared_request[0x00000001] = RUI7;
      }
      UI7::SameLine();
      if (UI7::Button("Font")) {
        shared_request[0x00000001] = RFV;
      }
      if (UI7::Button("Overlays")) {
        shared_request[0x00000001] = ROVERLAYS;
      }
      if (UI7::Button("IDB")) {
        shared_request[0x00000001] = RIDB;
      }
      if (UI7::Button("ThemeEditor")) {
        Palladium::LoadThemeEditor();
      }
      if (UI7::Button("Back")) {
        shared_request[0x00000002] = 1U;
      }
      if (UI7::Button("Keyboard")) {
        shared_request[0x00000003] = 1U;
      }
      UI7::EndMenu();
    }

  } else if (m_state == RIDB) {
    LI::OnScreen(false);
    if (UI7::BeginMenu("Palladium -> Debugger")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Server Running: " +
                 std::string(pdi_idb_running ? "true" : "false"));
      UI7::EndMenu();
    }
    LI::OnScreen(true);
    if (UI7::BeginMenu("Press B to go back!")) {
      if (UI7::Button("Start Server")) {
        Palladium::IDB::Start();
      }
      UI7::SameLine();
      if (UI7::Button("Stop Server")) {
        Palladium::IDB::Stop();
      }
      UI7::SameLine();
      if (UI7::Button("Restart Server")) {
        Palladium::IDB::Restart();
      }

      UI7::EndMenu();
    }

  } else if (m_state == RFTRACE) {
    LI::OnScreen(false);
    auto list = UI7::GetBackgroundList();
    list->Layer(10);
    int lrb = list->Layer();
    // Draw Top Screen Into Background DrawList
    list->AddRectangle(NVec2(0, 0), NVec2(400, 240), PDColor_Background);
    list->AddRectangle(NVec2(0, 0), NVec2(400, 20), PDColor_Header);
    list->Layer(lrb + 1);
    list->AddText(NVec2(5, 2), "Palladium -> FTrace",
                  Palladium::ThemeActive()->AutoText(PDColor_Header));
    list->AddText(NVec2(395, 2), PDVSTRING,
                  Palladium::ThemeActive()->AutoText(PDColor_Header),
                  PDTextFlags_AlignRight);
    list->Layer(lrb);
    list->AddRectangle(NVec2(0, 220), NVec2(400, 20),
                       Palladium::ThemeActive()->Get(PDColor_Header));
    list->Layer(lrb + 1);
    list->AddText(NVec2(5, 222),
                  "Traces: " + std::to_string(ftrace_index + 1) + "/" +
                      std::to_string(Palladium::Ftrace::pd_traces.size()),
                  Palladium::ThemeActive()->AutoText(PDColor_Header));
    list->Layer(lrb);
    list->AddRectangle(NVec2(0, 20), NVec2(400, 20), PDColor_TextDisabled);
    list->Layer(lrb + 1);
    list->AddText(NVec2(5, 22), "Function:",
                  Palladium::ThemeActive()->AutoText(PDColor_TextDisabled));
    list->AddText(NVec2(395, 22), "Time (ms):",
                  Palladium::ThemeActive()->AutoText(PDColor_TextDisabled),
                  PDTextFlags_AlignRight);
    list->Layer(lrb);
    // List Bg
    for (int i = 0; i < 12; i++) {
      if ((i % 2 == 0))
        list->AddRectangle(NVec2(0, 40 + (i)*15), NVec2(400, 15),
                           PDColor_List0);
      else
        list->AddRectangle(NVec2(0, 40 + (i)*15), NVec2(400, 15),
                           PDColor_List1);
    }

    Palladium::Ftrace::Beg("PDft", "display_traces");
    int start_index = ftrace_index < 11 ? 0 : ftrace_index - 11;
    auto it = Palladium::Ftrace::pd_traces.begin();
    std::advance(it, start_index);
    int ix = start_index;
    std::string _fkey__ = "0";

    while (ix < (int)Palladium::Ftrace::pd_traces.size() &&
           ix < start_index + 12 && it != Palladium::Ftrace::pd_traces.end()) {
      if (ix == ftrace_index) {
        _fkey__ = it->first;
        list->Layer(lrb + 1);
        list->AddRectangle(NVec2(0, 40 + (ix - start_index) * 15),
                           NVec2(400, 15), PDColor_Selector);
      }
      list->Layer(lrb + 2);
      auto clr = ix == ftrace_index
                     ? PDColor_Selector
                     : (ix % 2 == 0 ? PDColor_List0 : PDColor_List1);
      list->AddText(NVec2(5, 40 + (ix - start_index) * 15),
                    it->second.func_name,
                    Palladium::ThemeActive()->AutoText(clr));
      list->AddText(NVec2(395, 40 + (ix - start_index) * 15),
                    Palladium::MsTimeFmt(it->second.time_of),
                    Palladium::ThemeActive()->AutoText(clr),
                    PDTextFlags_AlignRight);
      ++it;
      ++ix;
    }

    Palladium::Ftrace::End("PDft", "display_traces");

    LI::OnScreen(true);
    if (UI7::BeginMenu("Press B to go back!")) {
      auto jt = Palladium::Ftrace::pd_traces.begin();
      std::advance(jt, ftrace_index);
      UI7::Label("Group: " + jt->second.group);
      UI7::Label("Function: " + jt->second.func_name);
      UI7::Checkbox("In Overlay", jt->second.is_ovl);
      UI7::Label("Time: " + Palladium::MsTimeFmt(jt->second.time_of));
      UI7::Label("TS: " + std::to_string(jt->second.time_start));
      UI7::Label("TE: " + std::to_string(jt->second.time_end));
      UI7::Label("SVC_Stk: " + std::to_string(svcGetSystemTick()));
      UI7::Label("Last 60 frames:");
      UI7::Label("Max: " + Palladium::MsTimeFmt(jt->second.ts.GetMax()));
      UI7::Label("Min: " + Palladium::MsTimeFmt(jt->second.ts.GetMax()));
      UI7::Label("Avg: " + Palladium::MsTimeFmt(jt->second.ts.GetMax()));
      UI7::EndMenu();
    }
  } else if (m_state == RUI7) {
    LI::OnScreen(false);
    if (UI7::BeginMenu("Palladium -> UI7")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Time: " + std::to_string(UI7::GetTime()));
      UI7::Label("Delta: " + std::to_string(UI7::GetDeltaTime()));
      UI7::Label("Hid Down Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Down)));
      UI7::Label("Hid Held Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Held)));
      UI7::Label("Hid Up Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Up)));
      UI7::Label("Touch Pos: " + std::to_string(Hid::GetTouchPosition().x()) +
                 ", " + std::to_string(Hid::GetTouchPosition().y()));
      UI7::Label(
          "Touch Last Pos: " + std::to_string(Hid::GetLastTouchPosition().x()) +
          ", " + std::to_string(Hid::GetLastTouchPosition().y()));
      UI7::Label(
          "Touch Down Pos: " + std::to_string(Hid::GetTouchDownPosition().x()) +
          ", " + std::to_string(Hid::GetTouchDownPosition().y()));
      UI7::EndMenu();
    }

    LI::OnScreen(true);
    if (UI7::BeginMenu("Press B to go back!", NVec2(),
                       UI7MenuFlags_Scrolling)) {
      if (UI7::Button("Go back")) {
        /// Request a state switch to state RSETTINGS
        shared_request[0x00000001] = RSETTINGS;
      }
      UI7::Checkbox("Debug", UI7::IsDebugging());
      UI7::Checkbox("ShowMenuInfo", UI7::DebugMenu());
      UI7::EndMenu();
    }
  } else if (m_state == ROVERLAYS) {
    LI::OnScreen(false);
    if (UI7::BeginMenu("Palladium -> Overlays")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Metrik Overlay: " + mtovlstate);
      UI7::Label("Metrik Screen: " + mtscreenstate);
      UI7::EndMenu();
    }

    LI::OnScreen(true);
    if (UI7::BeginMenu("Press B to go back!", NVec2(),
                       UI7MenuFlags_Scrolling)) {
      if (UI7::Button("Go back")) {
        /// Request a state switch to state RSETTINGS
        shared_request[0x00000001] = RSETTINGS;
      }
      UI7::Separator();
      UI7::Label("FTrace:");
      UI7::Checkbox("Enable Overlay", pdi_ftraced);
      UI7::Separator();
      UI7::Label("FTrace Flags:");
      auto &pd_ft_ovl = pd_ftrace_ovl_flags;
      OverlayFlag(pd_ft_ovl, PDFTraceOverlayFlags_DisplayName,
                  "Display Func Name");
      OverlayFlag(pd_ft_ovl, PDFTraceOverlayFlags_DisplayHelp, "Display Help");
      OverlayFlag(pd_ft_ovl, PDFTraceOverlayFlags_DisplayAverage,
                  "Average Time");
      OverlayFlag(pd_ft_ovl, PDFTraceOverlayFlags_DisplayMin, "Minimum Time");
      OverlayFlag(pd_ft_ovl, PDFTraceOverlayFlags_DisplayMax, "Maximum Time");
      OverlayFlag(pd_ft_ovl, PDFTraceOverlayFlags_FillBg, "Darker Background");
      UI7::Separator();
      UI7::Label("Metrik");
      UI7::Label("Format: Usage | Current Time -> Average");
      UI7::Checkbox("Enable Overlay", pdi_metrikd);
      UI7::Checkbox("Bottom Screen", pdi_mt_screen);
      UI7::ColorSelector("Text", pdi_mt_txtcolor);
      UI7::ColorSelector("Text Background", pdi_mt_color);
      UI7::Label("TextSize: " + std::to_string(pdi_mt_txtSize));
      UI7::SameLine();
      if (UI7::Button("+")) {
        pdi_mt_txtSize += 0.1;
      }
      UI7::SameLine();
      if (UI7::Button("-")) {
        pdi_mt_txtSize -= 0.1;
      }
      UI7::Separator();
      UI7::Label("Metrik Flags:");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_FPS,
                  "Application average");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_CPU, "CPU Usage");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_GPU, "GPU Usage");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_CMD, "Command Buf Usage");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_LMM, "Linear Space Free");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_LVT, "LI Vertices");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_LID, "LI Indices");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_LDM, "LI Draw Commands");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_LDC, "LI Draw Calls");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_PDO, "Display Info Line");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_MTD, "MemTrack Info");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_CGR, "Display CPU Graph");
      OverlayFlag(pd_ovl_flags, PDMetrikOverlayFlags_GGR, "Display GPU Graph");
      UI7::EndMenu();
    }
  } else if (m_state == RFV) {
    LI::OnScreen(false);
    if (UI7::BeginMenu("Palladium -> Font Viewer")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Font: " + LI::GetFont()->GetName());
      UI7::EndMenu();
    }

    LI::OnScreen(true);
    if (UI7::BeginMenu("Press B to go back!", NVec2(),
                       UI7MenuFlags_Scrolling)) {
      UI7::EndMenu();
    }
  }
  // Standart Logic
  /// Requests
  for (const auto &it : shared_request) {
    if (it.first == 0x00000001) {
      m_state = (RState)it.second;
    } else if (it.first == 0x00000002) {
      if (it.second) {
        std::fstream cfg_wrt(pdi_config_path + "/config.rc7", std::ios::out);
        pdi_config["metrik-settings"]["show"] = pdi_metrikd;
        pdi_config["metrik-settings"]["Screen"] = pdi_mt_screen;
        pdi_config["metrik-settings"]["config"] = pd_ovl_flags;
        pdi_config["metrik-settings"]["Text"] = pdi_mt_txtcolor;
        pdi_config["metrik-settings"]["Size"] = pdi_mt_txtSize;
        pdi_config["metrik-settings"]["Bg"] = pdi_mt_color;
        cfg_wrt << pdi_config.dump(4);
        cfg_wrt.close();
        pdi_settings = false;
        Hid::Clear();
        Palladium::Scene::Back();
        // Instant break logic or it will crash
        return;
      }
    } else if (it.first == 0x00000003) {
      if (it.second)
        Palladium::AddOvl(std::make_unique<Ovl_Keyboard>(kbd_test, kbd_state));
    }
  }
  /// Clear if handled
  shared_request.clear();

  if (statemtold != pdi_metrikd && pdi_metrikd == true)
    Palladium::AddOvl(std::make_unique<Ovl_Metrik>(
        &pdi_metrikd, &pdi_mt_screen, &pdi_mt_color, &pdi_mt_txtcolor,
        &pdi_mt_txtSize));
  statemtold = pdi_metrikd;
  if (stateftold != pdi_ftraced && pdi_ftraced == true)
    Palladium::AddOvl(std::make_unique<Ovl_Ftrace>(&pdi_ftraced));
  stateftold = pdi_ftraced;

  if (m_state == RSETTINGS) {
    if (d7_hUp & KEY_B) {
      std::fstream cfg_wrt(pdi_config_path + "/config.rc7", std::ios::out);
      pdi_config["metrik-settings"]["show"] = pdi_metrikd;
      pdi_config["metrik-settings"]["Screen"] = pdi_mt_screen;
      pdi_config["metrik-settings"]["config"] = pd_ovl_flags;
      pdi_config["metrik-settings"]["Text"] = pdi_mt_txtcolor;
      pdi_config["metrik-settings"]["Size"] = pdi_mt_txtSize;
      pdi_config["metrik-settings"]["Bg"] = pdi_mt_color;
      cfg_wrt << pdi_config.dump(4);
      cfg_wrt.close();
      pdi_settings = false;
      Hid::Clear();
      Palladium::Scene::Back();
    }
  }
  if (m_state == RUI7 || m_state == RFV) {
    if (d7_hUp & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == ROVERLAYS) {
    mtovlstate = pdi_metrikd ? "true" : "false";
    mtscreenstate = pdi_mt_screen ? "Bottom" : "Top";
    if (d7_hUp & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == RFTRACE) {
    if (d7_hDown & KEY_DOWN) {
      if (ftrace_index < (int)Palladium::Ftrace::pd_traces.size() - 1)
        ftrace_index++;
    }
    if (d7_hDown & KEY_UP) {
      if (ftrace_index > 0) ftrace_index--;
    }
    if (d7_hUp & KEY_B) {
      m_state = RSETTINGS;
    }
  }
}

void Palladium::LoadSettings() {
  if (!pdi_settings)
    Palladium::Scene::Load(std::make_unique<Palladium::RSettings>());
}

void Palladium::LoadThemeEditor() {
  Palladium::Scene::Load(std::make_unique<Palladium::ThemeEditor>());
}

void Palladium::AddOvl(std::unique_ptr<Palladium::Ovl> overlay) {
  pdi_overlays.push_back(std::move(overlay));
}

void Palladium::FadeOut() {
  if (!pdi_wait_fade) {
    pdi_fadein = true;
    pdi_fadealpha = 0;
    pdi_wait_fade = true;
  }
}

void Palladium::FadeIn() {
  if (!pdi_wait_fade) {
    pdi_fadeout = true;
    pdi_fadealpha = 255;
    pdi_wait_fade = true;
  }
}

void Palladium::FadeDisplay() { Npifade(); }

float Palladium::GetTime() { return pdi_time; }

std::string Palladium::GetAppDirectory() {
  std::string dir = "sdmc:/Palladium/Apps/" + pdi_app_name;
  if (!std::filesystem::is_directory(dir))
    std::filesystem::create_directories(dir);
  return dir;
}

std::string Palladium::GetDataDirectory() {
  std::string dir = GetAppDirectory() + "/data";
  if (!std::filesystem::is_directory(dir))
    std::filesystem::create_directories(dir);
  return dir;
}
