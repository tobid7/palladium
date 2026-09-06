#include <os/desktopos.hpp>
#include <os/horizon-ctr.hpp>
#include <os/horizon-nx.hpp>
#include <palladium>

PD::OsCtx* pOs = nullptr;

const char* ResourcePath(const char* in) {
#if defined(__SWITCH__) || defined(__3DS__)
  static char rbuf[512];
  std::snprintf(rbuf, sizeof(rbuf), "romfs:/%s", in);
  return rbuf;
#else
  return in;
#endif
}

int main(int argc, char** argv) {
  Driver drv = Driver::OpenGL3;
  if (argc == 2) {
    if (std::string(argv[1]) == "gl2") {
      drv = Driver::OpenGL2;
    } else if (std::string(argv[1]) == "gl3") {
      drv = Driver::OpenGL3;
    } else if (std::string(argv[1]) == "dx9") {
      drv = Driver::DirectX9;
    }
  }
#if defined(__SWITCH__)
  pOs = new PD::HorizonNX(drv);
#elif defined(__3DS__)
  pOs = new PD::HorizonCtr(drv);
#else
  pOs = new PD::DesktopOS(drv);
#endif
  pOs->Init();
  PD::Gfx::Init();
  PD::Li::Drawlist pList;
  while (pOs->Mainloop()) {
    pOs->ClearViewPort();
    PD::Li::ResetPools();
    pList.PathRect(pOs->SizeTranslate(0.05), pOs->SizeTranslate(0.4f), 10.f);
    pList.PathFill(0xff00ffff);
    PD::Gfx::Reset();
    PD::Gfx::Draw(pList);
    pList.Clear();
    pOs->SwapBuffers();
  }
  PD::Gfx::Deinit();
  pOs->Deinit();
  delete pOs;
  return 0;
}