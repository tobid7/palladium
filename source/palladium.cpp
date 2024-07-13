#include <pd/Hid.hpp>  // Integate HidApi
#include <pd/Message.hpp>
#include <pd/Overlays.hpp>
#include <pd/ThemeEditor.hpp>
#include <pd/UI7.hpp>
#include <pd/palladium.hpp>
#include <pd/LI7.hpp>

// Config 2
#include <pd/external/json.hpp>
#include <pd/internal_db.hpp>

// C++ includes
#include <filesystem>
#include <random>

Palladium::LoggerBase::Ref pdi_glogger;
extern Palladium::LoggerBase::Ref pdi_logger;

static void pdi_ExitHook() {
  C2D_TextBufDelete(pdi_text_buffer);
  C2D_TextBufDelete(pdi_d2_dimbuf);
  romfsExit();
}

std::vector<std::string> string_to_lines(std::string input_str) {
  std::vector<std::string> lines;
  std::stringstream ss(input_str);
  std::string line;
  while (std::getline(ss, line)) {
    lines.push_back(line);
  }
  return lines;
}

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
    Palladium::R2::OnScreen(Palladium::R2Screen_Top);
    Palladium::R2::AddRect(NVec2(0, 0), NVec2(400, 240),
                            ((pdi_fadealpha << 24) | 0x00000000));
    Palladium::R2::OnScreen(Palladium::R2Screen_Bottom);
    Palladium::R2::AddRect(NVec2(0, 0), NVec2(320, 240),
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
  pdi_config_path = "sdmc:/Palladium/Apps/";
  pdi_config_path += pdi_app_name;
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
    pdi_config["metrik-settings"]["Text"] = "#ffffffff";
    pdi_config["metrik-settings"]["Bg"] = "#aa000000";
    pdi_config["metrik-settings"]["Size"] = 0.7f;
    pdi_config["internal_logger"]["nowritetxt"] = true;
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
  pdi_lggrf = pdi_config["internal_logger"]["nowritetxt"].get<bool>();

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

Palladium::LoggerBase::Ref Palladium::Logger() {
  if (!pdi_glogger) {
    Palladium::Error("Logger Was Called before being Init!");
    // return schould not be reached then
  }
  return pdi_glogger;
}

float Palladium::GetDeltaTime() { return (float)pdi_dtm; }

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

void Palladium::Scene::doDraw() {
  Ftrace::ScopedTrace st("pd-core", f2s(Scene::doDraw));
  if (!Palladium::Scene::scenes.empty()) Palladium::Scene::scenes.top()->Draw();
}

void Palladium::Scene::doLogic() {
  Ftrace::ScopedTrace st("pd-core", f2s(Scene::doLogic));
  if (!Palladium::Scene::scenes.empty()) Palladium::Scene::scenes.top()->Logic();
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
  Palladium::Ftrace::ScopedTrace st("pd-core", f2s(MainLoop));
  if (!aptMainLoop()) return false;
  // Deltatime
  uint64_t currentTime = svcGetSystemTick();
  pdi_dtm = ((float)(currentTime / (float)TICKS_PER_MSEC) -
              (float)(pdi_last_tm / (float)TICKS_PER_MSEC)) /
             1000.f;
  pdi_time += pdi_dtm;
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

  Palladium::ClearTextBufs();
  C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

  C2D_TargetClear(pd_top, C2D_Color32(0, 0, 0, 0));
  C2D_TargetClear(pd_bottom, C2D_Color32(0, 0, 0, 0));
  frameloop();
  if (pdi_enable_scene_system) {
    Palladium::Scene::doDraw();
    Palladium::Scene::doLogic();
  }
  return pdi_running;
}

void Palladium::ClearTextBufs(void) { C2D_TextBufClear(pdi_text_buffer); }

void Palladium::Init::Graphics() {
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init((size_t)pd_max_objects);
  C2D_Prepare();
  pd_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  pd_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  pd_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  pdi_text_buffer = C2D_TextBufNew(4096);
  pdi_d2_dimbuf = C2D_TextBufNew(4096);
  pdi_base_font = C2D_FontLoadSystem(CFG_REGION_USA);
  R2::Init();
}

Result Palladium::Init::Main(std::string app_name) {
  Palladium::Ftrace::ScopedTrace st("pd-core", f2s(Init::Main));
  pdi_app_name = app_name;
  pdi_logger = LoggerBase::New();
  pdi_glogger = LoggerBase::New();

  pdi_enable_scene_system = (pd_flags & PDFlags_SceneSystem);
  pdi_enable_memtrack = (pd_flags & PDFlags_MemTrack);

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
  _pdi_logger()->Init("Palladium", pdi_lggrf);

  pdi_active_theme = Theme::New();
  pdi_active_theme->Default();

  auto ret = pdi_soc_init();
  if (ret) {
    pdi_logger->Write("Failed to Init Soc!");
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
  C2D_Init((size_t)pd_max_objects);
  atexit(C2D_Fini);
  atexit(pdi_ExitHook);
  C2D_Prepare();
  pd_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  pd_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  pd_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  pdi_text_buffer = C2D_TextBufNew(4096);
  pdi_d2_dimbuf = C2D_TextBufNew(4096);
  pdi_base_font = C2D_FontLoadSystem(CFG_REGION_USA);
  R2::Init();

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
  pdi_logger = LoggerBase::New();
  pdi_glogger = LoggerBase::New();

  pdi_enable_scene_system = (pd_flags & PDFlags_SceneSystem);
  pdi_enable_memtrack = (pd_flags & PDFlags_MemTrack);

  gfxInitDefault();
  atexit(gfxExit);
  romfsInit();

  pdi_init_config();
  _pdi_logger()->Init("Palladium", pdi_lggrf);

  pdi_active_theme = Theme::New();
  pdi_active_theme->Default();

  auto ret = pdi_soc_init();
  if (ret) {
    pdi_logger->Write("Failed to Init Soc!");
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
  C2D_Init((size_t)pd_max_objects);
  atexit(C2D_Fini);
  atexit(pdi_ExitHook);
  C2D_Prepare();
  pd_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  pd_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  pd_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  pdi_text_buffer = C2D_TextBufNew(4096);
  pdi_d2_dimbuf = C2D_TextBufNew(4096);
  pdi_base_font = C2D_FontLoadSystem(CFG_REGION_USA);
  R2::Init();

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
  pdi_graphics_on = false;
  C2D_TextBufDelete(pdi_text_buffer);
  C2D_Fini();
  C3D_Fini();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init((size_t)pd_max_objects);
  C2D_Prepare();
  pd_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
  pd_top_right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
  pd_bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
  pdi_text_buffer = C2D_TextBufNew(4096);
  pdi_base_font = C2D_FontLoadSystem(CFG_REGION_USA);
  R2::Init();
  pdi_graphics_on = true;

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
    Palladium::Scene::doDraw();
    Palladium::Scene::doLogic();
  }
  UI7::Update();
  UI7::Debug();
  Palladium::ProcessMessages();
  OvlHandler();
  Npifade();
  R2::Process();
  C3D_FrameEnd(0);
}

Palladium::RSettings::RSettings() {
  // Palladium Settings is designed for
  // System Font
  R2::DefaultFont();
  tmp_txt = R2::GetTextSize();
  R2::DefaultTextSize();
  Palladium::FadeIn();
  std::fstream cfg_ldr(pdi_config_path + "/config.rc7", std::ios::in);
  cfg_ldr >> pdi_config;
  cfg_ldr.close();
  pdi_settings = true;
  statemtold = pdi_metrikd;
  stateftold = pdi_ftraced;
}

Palladium::RSettings::~RSettings() { R2::SetTextSize(tmp_txt); }

std::vector<std::string> StrHelper(std::string input) {
  std::string ss(input);
  std::istringstream in(ss);
  std::vector<std::string> test1;

  std::copy(std::istream_iterator<std::string>(in),
            std::istream_iterator<std::string>(), std::back_inserter(test1));
  return test1;
}

void Palladium::RSettings::Draw(void) const {
  if (m_state == RSETTINGS) {
    Palladium::R2::OnScreen(R2Screen_Top);
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
      UI7::Label("Kbd test: " + kbd_test);
      UI7::EndMenu();
    }
    Palladium::R2::OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!")) {
      if (UI7::Button("FTrace")) {
        shared_request[0x00000001] = RFTRACE;
      }
      if (UI7::Button("UI7")) {
        shared_request[0x00000001] = RUI7;
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
      if (UI7::Button("Logs")) {
        shared_request[0x00000001] = RLOGS;
      }
      UI7::SameLine();
      UI7::Checkbox("No File", pdi_lggrf);
      if (UI7::Button("Back")) {
        shared_request[0x00000002] = 1U;
      }
      if (UI7::Button("Keyboard")) {
        shared_request[0x00000003] = 1U;
      }
      UI7::EndMenu();
    }

  } else if (m_state == RIDB) {
    Palladium::R2::OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("Palladium -> Debugger")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Server Running: " +
                 std::string(pdi_idb_running ? "true" : "false"));
      UI7::EndMenu();
    }
    Palladium::R2::OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!")) {
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
    Palladium::R2::OnScreen(R2Screen_Top);
    // Draw Top Screen Into Background DrawList
    UI7::GetBackgroundList()->AddRectangle(NVec2(0, 0), NVec2(400, 240),
                                           PDColor_Background);
    UI7::GetBackgroundList()->AddRectangle(NVec2(0, 0), NVec2(400, 20),
                                           PDColor_Header);
    UI7::GetBackgroundList()->AddText(
        NVec2(5, 2), "Palladium -> FTrace",
        Palladium::ThemeActive()->AutoText(PDColor_Header));
    UI7::GetBackgroundList()->AddText(
        NVec2(395, 2), PDVSTRING,
        Palladium::ThemeActive()->AutoText(PDColor_Header),
        PDTextFlags_AlignRight);
    UI7::GetBackgroundList()->AddRectangle(
        NVec2(0, 220), NVec2(400, 20),
        Palladium::ThemeActive()->Get(PDColor_Header));
    UI7::GetBackgroundList()->AddText(
        NVec2(5, 222),
        "Traces: " + std::to_string(ftrace_index + 1) + "/" +
            std::to_string(Palladium::Ftrace::pd_traces.size()),
        Palladium::ThemeActive()->AutoText(PDColor_Header));
    UI7::GetBackgroundList()->AddRectangle(NVec2(0, 20), NVec2(400, 20),
                                           PDColor_TextDisabled);
    UI7::GetBackgroundList()->AddText(
        NVec2(5, 22),
        "Function:", Palladium::ThemeActive()->AutoText(PDColor_TextDisabled));
    UI7::GetBackgroundList()->AddText(
        NVec2(395, 22),
        "Time (ms):", Palladium::ThemeActive()->AutoText(PDColor_TextDisabled),
        PDTextFlags_AlignRight);

    // List Bg
    for (int i = 0; i < 12; i++) {
      if ((i % 2 == 0))
        UI7::GetBackgroundList()->AddRectangle(NVec2(0, 40 + (i) * 15),
                                               NVec2(400, 15), PDColor_List0);
      else
        UI7::GetBackgroundList()->AddRectangle(NVec2(0, 40 + (i) * 15),
                                               NVec2(400, 15), PDColor_List1);
    }

    Palladium::Ftrace::Beg("PDft", "display_traces");
    int start_index = ftrace_index < 11 ? 0 : ftrace_index - 11;
    auto it = Palladium::Ftrace::pd_traces.begin();
    std::advance(it, start_index);
    int ix = start_index;
    std::string _fkey__ = "0";

    while (ix < (int)Palladium::Ftrace::pd_traces.size() &&
           ix < start_index + 10 && it != Palladium::Ftrace::pd_traces.end()) {
      if (ix == ftrace_index) {
        _fkey__ = it->first;
        UI7::GetBackgroundList()->AddRectangle(
            NVec2(0, 40 + (ix - start_index) * 15), NVec2(400, 15),
            PDColor_Selector);
      }
      auto clr = ix == ftrace_index
                     ? PDColor_Selector
                     : (ix % 2 == 0 ? PDColor_List0 : PDColor_List1);
      UI7::GetBackgroundList()->AddText(NVec2(5, 40 + (ix - start_index) * 15),
                                        it->second.func_name,
                                        Palladium::ThemeActive()->AutoText(clr));
      UI7::GetBackgroundList()->AddText(
          NVec2(395, 40 + (ix - start_index) * 15),
          Palladium::MsTimeFmt(it->second.time_of),
          Palladium::ThemeActive()->AutoText(clr), PDTextFlags_AlignRight);
      ++it;
      ++ix;
    }

    Palladium::Ftrace::End("PDft", "display_traces");

    Palladium::R2::OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!")) {
      auto jt = Palladium::Ftrace::pd_traces.begin();
      std::advance(jt, ftrace_index);
      UI7::Label("Group: " + jt->second.group);
      UI7::Label("Function: " + jt->second.func_name);
      UI7::Checkbox("In Overlay", jt->second.is_ovl);
      UI7::Label("Time: " + Palladium::MsTimeFmt(jt->second.time_of));
      UI7::Label("Max: " + Palladium::MsTimeFmt(jt->second.time_ofm));
      UI7::Label("TS: " + std::to_string(jt->second.time_start));
      UI7::Label("TE: " + std::to_string(jt->second.time_end));
      UI7::Label("SVC_Stk: " + std::to_string(svcGetSystemTick()));
      UI7::EndMenu();
    }
  } else if (m_state == RUI7) {
    Palladium::R2::OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("Palladium -> UI7")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Time: " + std::to_string(UI7::GetTime()));
      UI7::Label("Delta: " + std::to_string(UI7::GetDeltaTime() * 1000.f));
      UI7::Label("Hid Down Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Down)));
      UI7::Label("Hid Held Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Held)));
      UI7::Label("Hid Up Touch: " +
                 std::to_string(Hid::IsEvent("touch", Hid::Up)));
      UI7::Label("Touch Pos: " + std::to_string(Hid::GetTouchPosition().x) +
                 ", " + std::to_string(Hid::GetTouchPosition().y));
      UI7::Label(
          "Touch Last Pos: " + std::to_string(Hid::GetLastTouchPosition().x) +
          ", " + std::to_string(Hid::GetLastTouchPosition().y));
      UI7::Label(
          "Touch Down Pos: " + std::to_string(Hid::GetTouchDownPosition().x) +
          ", " + std::to_string(Hid::GetTouchDownPosition().y));
      UI7::EndMenu();
    }

    Palladium::R2::OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!", NVec2(),
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
    Palladium::R2::OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("Palladium -> Overlays")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::Label("Metrik Overlay: " + mtovlstate);
      UI7::Label("Metrik Screen: " + mtscreenstate);
      UI7::EndMenu();
    }

    Palladium::R2::OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!")) {
      UI7::Label("Metrik:");
      UI7::Checkbox("Enable Overlay", pdi_metrikd);
      UI7::Checkbox("Bottom Screen", pdi_mt_screen);
      UI7::Label("FTrace:");
      UI7::Checkbox("Enable Overlay", pdi_ftraced);
      UI7::SetCursorPos(NVec2(5, 215));
      if (UI7::Button("Go back")) {
        /// Request a state switch to state RSETTINGS
        shared_request[0x00000001] = RSETTINGS;
      }
      UI7::EndMenu();
    }
  } else if (m_state == RLOGS) {
    Palladium::R2::OnScreen(R2Screen_Top);
    if (UI7::BeginMenu("Palladium -> Logs")) {
      UI7::SetCursorPos(NVec2(395, 2));
      UI7::Label(PDVSTRING, PDTextFlags_AlignRight);
      UI7::RestoreCursor();
      UI7::EndMenu();
    }

    Palladium::R2::OnScreen(R2Screen_Bottom);
    if (UI7::BeginMenu("Press \uE001 to go back!", NVec2(),
                       UI7MenuFlags_Scrolling)) {
      for (auto &it : pdi_logger->Lines()) UI7::Label(it, PDTextFlags_Wrap);
      UI7::EndMenu();
    }
  }
}

void Palladium::RSettings::Logic() {
  /// Requests
  for (const auto &it : shared_request) {
    if (it.first == 0x00000001) {
      m_state = (RState)it.second;
    } else if (it.first == 0x00000002) {
      if (it.second) {
        std::fstream cfg_wrt(pdi_config_path + "/config.rc7", std::ios::out);
        pdi_config["metrik-settings"]["enableoverlay"] = pdi_metrikd;
        pdi_config["metrik-settings"]["Screen"] = pdi_mt_screen;
        pdi_config["internal_logger"]["nowritetxt"] = pdi_lggrf;
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
    if (d7_hDown & KEY_B) {
      std::fstream cfg_wrt(pdi_config_path + "/config.rc7", std::ios::out);
      pdi_config["metrik-settings"]["enableoverlay"] = pdi_metrikd;
      pdi_config["metrik-settings"]["Screen"] = pdi_mt_screen;
      pdi_config["internal_logger"]["nowritetxt"] = pdi_lggrf;
      cfg_wrt << pdi_config.dump(4);
      cfg_wrt.close();
      pdi_settings = false;
      Hid::Clear();
      Palladium::Scene::Back();
    }
  }
  if (m_state == RUI7) {
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == ROVERLAYS) {
    mtovlstate = pdi_metrikd ? "true" : "false";
    mtscreenstate = pdi_mt_screen ? "Bottom" : "Top";
    if (d7_hDown & KEY_B) {
      m_state = RSETTINGS;
    }
  }
  if (m_state == RIDB || m_state == RLOGS) {
    if (d7_hDown & KEY_B) {
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
    if (d7_hDown & KEY_B) {
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
