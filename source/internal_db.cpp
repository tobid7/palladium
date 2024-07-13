#include <arpa/inet.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pd/Error.hpp>
#include <pd/FileSystem.hpp>
#include <pd/external/json.hpp>
#include <pd/internal_db.hpp>
#include <pd/palladium.hpp>

/// Base ///
PDFlags pd_flags = PDFlags_Default;
static Palladium::Thread pdi_idb_server;  // Protected
std::string pdi_app_name;
std::string pdi_config_path;
nlohmann::json pdi_config;
u8 pdi_console_model = 0;
u8 pdi_system_region = CFG_REGION_USA;
bool pdi_is_citra = false;
bool pdi_settings = false;
NVec2 pdi_hid_touch_pos;
C2D_TextBuf pdi_text_buffer;
C2D_Font pdi_base_font;
C2D_TextBuf pdi_d2_dimbuf;
bool pdi_is_ndsp = false;
bool pdi_running = false;
std::stack<std::unique_ptr<Palladium::Scene>> Palladium::Scene::scenes;
std::unique_ptr<Palladium::Scene> pdi_fade_scene;
std::vector<std::unique_ptr<Palladium::Ovl>> pdi_overlays;
unsigned int pdi_frames = 0;
u64 pdi_last_time = 0;
float pdi_framerate = 0.0f;
u32 pdi_mt_color = 0xaa000000;
u32 pdi_mt_txtcolor = 0xbbffffff;
bool pdi_mt_screen;
float pdi_mt_txtSize;
bool pdi_metrikd = false;
bool pdi_ftraced = false;
u64 pdi_delta_time;
u64 pdi_last_tm;
float pdi_dtm;
float pdi_time;
bool pdi_fadeout = false, pdi_fadein = false, pdi_fadeout2 = false,
     pdi_fadein2 = false;
int pdi_fadealpha = 0;
int pdi_fadecolor = 0;
bool pdi_wait_fade = false;
bool pdi_fade_exit = false;
bool pdi_fade_scene_wait = false;
bool pdi_idb_running = false;
bool pdi_graphics_on = false;
float pd_draw2_tsm = 1.2f;
bool pdi_amdt = false;
void *pdi_soc_buf = nullptr;
bool pdi_is_am_init = false;
Palladium::Theme::Ref pdi_active_theme;
Palladium::LoggerBase::Ref pdi_logger;
bool pdi_lggrf = false;

Palladium::LoggerBase::Ref _pdi_logger() {
  if (!pdi_logger) {
    Palladium::Error(
        "You're trying to use a Palladium Func without Init Palladium!");
  }
  return pdi_logger;
}

/// Global ///
// Outdated HidApi (HidV2Patched)
u32 d7_hDown;
u32 d7_hHeld;
u32 d7_hUp;
u32 d7_hRepeat;  // Inofficial lol
touchPosition d7_touch;

// Modern Global Api
int pd_max_objects = C2D_DEFAULT_MAX_OBJECTS;
bool pdi_enable_scene_system = true;
bool pdi_debugging = false;
C3D_RenderTarget *pd_top;
C3D_RenderTarget *pd_top_right;
C3D_RenderTarget *pd_bottom;

Palladium::Net::Error pdi_soc_init() {
  if (pdi_soc_buf != nullptr) {
    return 0;
  }
  pdi_soc_buf = memalign(0x1000, 0x100000);
  if (!pdi_soc_buf) {
    return Palladium::Net::Error_Memory;
  }
  Result ret = socInit((u32 *)pdi_soc_buf, 0x100000);
  if (R_FAILED(ret)) {
    free(pdi_soc_buf);
    return ((static_cast<Palladium::Net::Error>(ret) << 32) |
            static_cast<Palladium::Net::Error>(Palladium::Net::Error_CtrStatus));
  }
  return 0;
}

void pdi_soc_deinit() {
  if (pdi_soc_buf) {
    socExit();
    free(pdi_soc_buf);
  }
  pdi_soc_buf = nullptr;
}

class Logger {
 public:
  Logger() = default;
  virtual ~Logger() = default;

  static void log(const std::string &prefix, const std::string &message) {
    std::cout << "[" << prefix << "]: " << message << std::endl;
  }
};

#define pd_err(x) Logger::log("ERROR", x)
#define pd_wrn(x) Logger::log("WARNING", x)

class tcp_server {
 public:
  tcp_server(const std::string &ip, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      pd_err("Unable to create socket!");
      return;
    }
    server.sin_family = AF_INET;
    server.sin_port = port;
    server.sin_addr.s_addr = inet_addr(ip.c_str());

    e = bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (e == -1) {
      pd_err("Unable to bind!");
      return;
    }
    e = listen(sockfd, 10);
    if (e == 0) {
      printf("[+]Listening...\n");
    } else {
      pd_err("Error in Binding");
      return;
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
    std::cout << "Connected" << std::endl;
  }
  ~tcp_server() {
    // Nothing here
  }

  bool reconnect() {
    close(new_sock);
    new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
    return true;
  }

  size_t snd(void *ptr, size_t size) { return send(new_sock, ptr, size, 0); }

  size_t rec(void *ptr, size_t size) { return recv(new_sock, ptr, size, 0); }

 private:
  int e, sockfd, new_sock;
  struct sockaddr_in server, new_addr;
  socklen_t addr_size;
};

#define stupid(x) &x, sizeof(x)
#define pdi_reacttion(x)     \
  {                           \
    int code = x;             \
    server.snd(stupid(code)); \
  }

struct pak32 {
  pak32() {}
  pak32(const std::string &n0, float n1, unsigned char n2, unsigned char n3,
        bool n4, bool n5, bool n6, float n7, float n8, float n9, float n10,
        unsigned int n11, unsigned int n12, unsigned int n13,
        unsigned int n14) {
    magic = 0x44772277;
    for (int i = 0; i < 64; i++) app_name[i] = (char)0;
    int l = n0.length();
    if (l > 64) l = 64;
    for (int i = 0; i < l; i++) app_name[i] = n0[i];
    framerate = n1;
    console_model = n2;
    system_region = n3;
    is_citra = n4;
    ndsp_support = n5;
    in_settings = n6;
    deltatime = n7;
    rtime = n8;
    cpu = n9;
    gpu = n10;
    mem_alloc = n11;
    mem_dalloc = n12;
    mem_ialloc = n13;
    tbs = n14;
  }
  uint32_t magic;
  char app_name[64];
  float framerate;
  unsigned char console_model;
  unsigned char system_region;
  bool is_citra;
  bool ndsp_support;
  bool in_settings;
  float deltatime;
  float rtime;
  float cpu;
  float gpu;
  unsigned int mem_alloc;
  unsigned int mem_dalloc;
  unsigned int mem_ialloc;
  unsigned int tbs;
};

static bool pdi_idb_fp = false;

void KillIdbServer() {
  pdi_idb_fp = true;
  pdi_idb_server.join(100);
}

void ServerThread(Palladium::Parameter param) {
  if (pdi_soc_buf == nullptr) return;
  pdi_idb_running = true;
  pdi_idb_fp = false;
  atexit(KillIdbServer);
  tcp_server server("0.0.0.0", 4727);
  int cmd = 0;
  while (!pdi_idb_fp) {
    size_t r = server.rec(&cmd, sizeof(int));
    if (r == 0) {
      server.reconnect();
    }
    if (cmd == 1) {
      pdi_reacttion(1);
      auto pak = pak32(
          pdi_app_name, pdi_framerate, pdi_console_model, pdi_system_region,
          pdi_is_citra, pdi_is_ndsp, pdi_settings, pdi_dtm, pdi_time,
          C3D_GetProcessingTime(), C3D_GetDrawingTime(),
          Palladium::Memory::GetTotalAllocated(),
          Palladium::Memory::GetTotalFreed(), Palladium::Memory::GetCurrent(),
          C2D_TextBufGetNumGlyphs(pdi_text_buffer));
      server.snd(stupid(pak));
    } else if (cmd == 2) {
      pdi_reacttion(2);
      std::vector<Palladium::FileSystem::Entry> el =
          Palladium::FileSystem::GetDirContent("sdmc:/");
      size_t buf = el.size() * sizeof(Palladium::FileSystem::Entry);
      server.snd(stupid(buf));
      server.snd(el.data(), buf);

    } else if (cmd == 3) {
      pdi_reacttion(3);
      Palladium::ExitApp();
      pdi_idb_fp = true;
    } else {
      pdi_reacttion(1234);
    }
  }
  pdi_idb_running = false;
}

namespace Palladium {
namespace IDB {
void Start() {
  if (pdi_idb_running) return;
  pdi_idb_server.initialize(ServerThread);
  pdi_idb_server.start(true);
}
void Stop() {
  if (!pdi_idb_running) return;
  pdi_idb_fp = true;
  KillIdbServer();
}
void Restart() {
  Stop();
  Start();
}
}  // namespace IDB
}  // namespace Palladium