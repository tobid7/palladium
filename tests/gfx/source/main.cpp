#include <os/desktopos.hpp>
#include <os/horizon-ctr.hpp>
#include <os/horizon-nx.hpp>
#include <palladium>

////
#include <pd/ultra/elems/image.hpp>
#include <pd/ultra/elems/rect.hpp>
#include <pd/ultra/elems/text.hpp>
#include <pd/ultra/layout.hpp>
////

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
  // PD::LogFilter(PD::LogLevel::Warning);
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
  PD::Image img(ResourcePath("icon.png"));
  auto pTex = PD::Gfx::LoadTexture(img, img.Width(), img.Height());
  PD::Li::Font font;
  font.LoadTTF(ResourcePath("default.ttf"), 64);
  pList.SetFont(&font);
  PD::Ultra::Layout lyt;
  lyt.SetViewport(PD::fvec2(1280, 720));
  lyt.SetFont(font);
  PD::Ultra::Rect r;
  r.SetColor(0xff0000ff);
  r.SetRounding(10.f);
  r.SetLined(true);
  r.SetPosition(250, 150);
  r.SetSize(100, 70);
  PD::Ultra::Rect rr;
  r.SetColor(0xff0000ff);
  r.SetRounding(10.f);
  r.SetPosition(250, 150);
  r.SetSize(100, 70);
  lyt.Push(&rr);
  lyt.Push(&r);
  PD::Ultra::Text txt;
  txt.SetPosition(PD::fvec2(5, 200));
  txt.SetText("OpenGL");
  txt.SetColor(PD::Color("#ffffffff"));
  lyt.Push(&txt);
  while (pOs->Mainloop()) {
    pOs->ClearViewPort();
    PD::Li::ResetPools();
    pList.DrawRectFilled(150, 50, 0x88ffffff);
    pList.DrawRect(150, 50, 0xffffffff);
    lyt.Render();
    pList.DrawText(
        5,
        std::format(
            "Font Scale: {}\nVP: [{}]\nVIDC: [{}, {}, {}, {}]\nGfxDriver: {}",
            pList.GetFontScale(), pOs->GetViewport(), PD::Gfx::GetNumVertices(),
            PD::Gfx::GetNumIndices(), PD::Gfx::GetNumDrawcalls(),
            PD::Gfx::GetNumCommands(), PD::Gfx::GetDriverName())
            .c_str(),
        0xffffffff);
    PD::Gfx::Reset();
    PD::Gfx::Draw(pList);
    PD::Gfx::Draw(lyt.Data());
    pList.Clear();
    pOs->SwapBuffers();
  }
  font.Delete();
  PD::Gfx::DeleteTexture(pTex);
  PD::Gfx::Deinit();
  pOs->Deinit();
  delete pOs;
  return 0;
}