#pragma once
#include <pd/Net.hpp>
#include <pd/external/json.hpp>
#include <pd/global_db.hpp>
#include <pd/palladium.hpp>

#define CFGVER "1"
#define THEMEVER "0"

#ifndef V_PDBTIME
#define V_PDBTIME "SUBMODULE"
#endif
#ifndef V_PDCSTRING
#define V_PDCSTRING "SUBMODULE"
#endif

// Base
extern bool pdi_enable_scene_system;
extern bool pdi_debugging;
extern bool pdi_enable_memtrack;
extern std::string pdi_app_name;
extern std::string pdi_config_path;
extern nlohmann::json pdi_config;
extern u8 pdi_console_model;
extern u8 pdi_system_region;
extern bool pdi_is_citra;
extern bool pdi_settings;
extern NVec2 pdi_hid_touch_pos;
extern C2D_TextBuf pdi_text_buffer;
extern C2D_TextBuf pdi_d2_dimbuf;
extern C2D_Font pdi_base_font;
extern bool pdi_is_ndsp;
extern bool pdi_running;
extern std::unique_ptr<Palladium::Scene> pdi_fade_scene;
extern std::vector<std::unique_ptr<Palladium::Ovl>> pdi_overlays;
extern unsigned int pdi_frames;
extern u64 pdi_last_time;
extern float pdi_framerate;
extern u32 pdi_mt_color;
extern u32 pdi_mt_txtcolor;
extern bool pdi_mt_screen;
extern float pdi_mt_txtSize;
extern bool pdi_metrikd;
extern bool pdi_ftraced;
extern u64 pdi_delta_time;
extern u64 pdi_last_tm;
extern float pdi_dtm;
extern float pdi_time;
extern bool pdi_fadeout;
extern bool pdi_fadein;
extern bool pdi_fadeout2;
extern bool pdi_fadein2;
extern int pdi_fadealpha;
extern int pdi_fadecolor;
extern bool pdi_wait_fade;
extern bool pdi_fade_exit;
extern bool pdi_fade_scene_wait;
extern bool pdi_idb_running;
extern bool pdi_graphics_on;
extern bool pdi_amdt;
extern void* pdi_soc_buf;
extern bool pdi_is_am_init;
extern Palladium::Theme::Ref pdi_active_theme;
extern bool pdi_lggrf;

// Use function for protection
Palladium::LoggerBase::Ref _pdi_logger();
Palladium::Net::Error pdi_soc_init();
void pdi_soc_deinit();