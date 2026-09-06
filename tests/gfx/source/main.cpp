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

class MainMenu : public PD::Ultra::Layout {
 public:
  MainMenu(PD::Li::Font& font) {
    SetBaseViewport(PD::ivec2(1280, 720));
    pBackground.SetSize(800, 450);
    pBackground.SetColor(PD::Color("#ffffffff"));
    pBackground.SetAlignment(UltraAlignment_CenterHorizontal |
                             UltraAlignment_CenterVertical);
    Push(pBackground);
    pText.SetColor(PD::Color("#000000"));
    pText.SetText("Hello World");
    pText.SetAlignment(UltraAlignment_CenterHorizontal |
                       UltraAlignment_CenterVertical);
    pText.SetFont(font);
    Push(pText);
  }
  ~MainMenu() {}

 private:
  PD::Ultra::Rect pBackground;
  PD::Ultra::Text pText;
};

class App {
 public:
  App(PD::Li::Font& font) : main(font) {}
  ~App() {}

  void Update(PD::ivec2 vp, PD::Li::Drawlist& list) {
    main.SetViewport(vp);
    main.Render(list);
  }

 private:
  MainMenu main;
};

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
  App app(font);
  while (pOs->Mainloop()) {
    PD::Hid::Update();
    pOs->ClearViewPort();
    PD::Li::ResetPools();  // Move to other place (or refactor this)
    app.Update(pOs->GetViewport(), pList);
    pList.DrawText(5, std::format("Mouse: {}", PD::Hid::MousePos()).c_str(),
                   PD::Color("#ffffffff"));
    PD::Gfx::Reset();
    PD::Gfx::Draw(pList);
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