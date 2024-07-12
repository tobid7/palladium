#include <3ds.h>

#include <pd/Error.hpp>
#include <pd/UI7.hpp>
#include <pd/internal_db.hpp>
#include <pd/palladium.hpp>

void pdi_save_report(const std::string& msg) {
  auto ts = Palladium::GetTimeStr();
  std::ofstream f("sdmc:/Palladium/Reports/report_" + ts + ".txt");
  f << "Palladium Error [" << pdi_app_name << ", " << ts << "]" << std::endl;
  f << "Error Message: " << std::endl;
  f << msg << std::endl;
  f << "SysInfo: " << std::endl;
  f << "- Citra -> " << (pdi_is_citra ? "true" : "false") << std::endl;
  f.close();
}

namespace Palladium {
void Error(const std::string& msg) {
  pdi_save_report(msg);
  if (pdi_graphics_on) {
    while (aptMainLoop()) {
      hidScanInput();
      if (hidKeysDown() & KEY_START) break;
      C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
      Palladium::ClearTextBufs();
      C2D_TargetClear(pd_top, 0x00000000);
      C2D_TargetClear(pd_bottom, 0x00000000);
      Palladium::R2::OnScreen(R2Screen_Top);
      if (UI7::BeginMenu("Palladium - Error Manager", NVec2(),
                         UI7MenuFlags_TitleMid)) {
        UI7::Label(msg, PDTextFlags_Wrap);
        UI7::Label("Press Start to Exit!");
        UI7::EndMenu();
      }
      Palladium::R2::OnScreen(R2Screen_Bottom);
      UI7::Update();
      Palladium::R2::Process();
      C3D_FrameEnd(0);
    }
    exit(0);
  } else {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printf("Palladium - ERROR MANAGER\n\n%s\n", msg.c_str());
    printf("Report Saved in\nsdmc:/Palladium/Reports\n");
    printf("Press Start to Exit\n");
    while (aptMainLoop()) {
      hidScanInput();
      if (hidKeysDown() & KEY_START) break;
      gfxSwapBuffers();
    }
    gfxExit();
    exit(0);
  }
}
}  // namespace Palladium