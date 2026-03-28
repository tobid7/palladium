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

void DrawTextHaxxed(const char* text, PD::Li::Font& font, PD::Li::Drawlist& l,
                    const PD::Ultra::Canvas& c, const PD::fvec2& pos,
                    const PD::Color& color, UltraAlignment align) {
  PD::fvec2 bounds = font.GetTextBounds(text, l.GetFontScale());
  PD::Li::Rect _b = c.VTranslateObject(pos, bounds, align, true);
  l.UnbindTexture();
  l.PathRect(_b.TopLeft(), _b.BotRight());
  l.PathFill(0x30ff00ff);
  l.PathRect(_b.TopLeft(), _b.BotRight());
  l.PathStroke(0xffff00ff, 1, LiDrawFlags_Close);
  l.DrawText(c.VTranslateObject(pos, bounds, align, true).TopLeft(), text,
             color);
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
  lyt.GetCanvas().SetVirtualViewport(PD::fvec2(400, 240));
  lyt.SetFont(font);
  PD::Ultra::Rect rr(
      0, 0, 90, 60, PD::Color("#ffff00"), 12.f,
      UltraAlignment_CenterHorizontal | UltraAlignment_CenterVertical);
  PD::Ultra::Text txt;
  PD::Ultra::Image _img(&pTex, 10, 10, 5.f, UltraAlignment_BotRight);
  _img.SetSize(90);  // Override size
  lyt.Add(rr);
  lyt.Add(_img);
  txt.SetPosition(0);
  txt.SetColor(0xffffffff);
  txt.SetText("const std::string &text");
  txt.SetAlignment(UltraAlignment_TopLeft);
  lyt.Add(txt);
  while (pOs->Mainloop()) {
    lyt.GetCanvas().SetViewport(pOs->GetViewport());
    pOs->ClearViewPort();
    PD::Li::ResetPools();
    pList.SetFontscale(lyt.GetCanvas().VTranslateFontscale(0.7f));
    lyt.Render();
    /*auto t = lyt.GetCanvas().VTranslateObject(
        PD::fvec2(5, 30), PD::fvec2(170, 110), UltraAlignment_TopLeft);
    auto icnpos = lyt.GetCanvas().VTranslateObject(
        PD::fvec2(5, 5), PD::fvec2(60), UltraAlignment_TopRight);
    pList.PathRect(t.TopLeft(), t.BotRight(),
                   lyt.GetCanvas().VTranslateSize(10).x);
    pList.PathFill(0xff00ffff);
    pList.BindTexture(pTex);
    pList.PathRect(icnpos.TopLeft(), icnpos.BotRight());
    pList.PathFill(0xffffffff);
    DrawTextHaxxed("Hello World!", font, pList, lyt.GetCanvas(),
                   PD::fvec2(5, 5), 0xff0000ff, UltraAlignment_TopLeft);*/
    DrawTextHaxxed(
        std::format(
            "Font Scale: {}\nVP: [{}]\nVIDC: [{}, {}, {}, {}]\nGfxDriver: {}",
            pList.GetFontScale(), pOs->GetViewport(), PD::Gfx::GetNumVertices(),
            PD::Gfx::GetNumIndices(), PD::Gfx::GetNumDrawcalls(),
            PD::Gfx::GetNumCommands(), PD::Gfx::GetDriverName())
            .c_str(),
        font, pList, lyt.GetCanvas(), PD::fvec2(5, 5), 0xffffffff,
        UltraAlignment_BotLeft);
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