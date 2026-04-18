#include <os/desktopos.hpp>
#include <os/horizon-ctr.hpp>
#include <os/horizon-nx.hpp>
#include <palladium>

////
#include <pd/ultra/elems/button.hpp>
#include <pd/ultra/elems/element.hpp>
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

std::string Key2String(PD::Hid::Gamepad gp) {
  std::string res;
  if (gp & PD::Hid::Gamepad::A) {
    res += "A";
  }
  if (gp & PD::Hid::Gamepad::B) {
    res += "B";
  }
  if (gp & PD::Hid::Gamepad::X) {
    res += "X";
  }
  if (gp & PD::Hid::Gamepad::Y) {
    res += "Y";
  }
  if (gp & PD::Hid::Gamepad::Start) {
    res += "Start";
  }
  if (gp & PD::Hid::Gamepad::Select) {
    res += "Select";
  }
  if (gp & PD::Hid::Gamepad::DDown) {
    res += "DDown";
  }
  if (gp & PD::Hid::Gamepad::DUp) {
    res += "DUp";
  }
  if (gp & PD::Hid::Gamepad::DLeft) {
    res += "DLeft";
  }
  if (gp & PD::Hid::Gamepad::DRight) {
    res += "DRight";
  }
  if (gp & PD::Hid::Gamepad::CPDown) {
    res += "CPDown";
  }
  if (gp & PD::Hid::Gamepad::CPUp) {
    res += "CPUp";
  }
  if (gp & PD::Hid::Gamepad::CPLeft) {
    res += "CPLeft";
  }
  if (gp & PD::Hid::Gamepad::CPRight) {
    res += "CPRight";
  }
  if (gp & PD::Hid::Gamepad::CSDown) {
    res += "CSDown";
  }
  if (gp & PD::Hid::Gamepad::CSUp) {
    res += "CSUp";
  }
  if (gp & PD::Hid::Gamepad::CSLeft) {
    res += "CSLeft";
  }
  if (gp & PD::Hid::Gamepad::CSRight) {
    res += "CSRight";
  }
  if (gp & PD::Hid::Gamepad::L) {
    res += "L";
  }
  if (gp & PD::Hid::Gamepad::R) {
    res += "R";
  }
  if (gp & PD::Hid::Gamepad::ZL) {
    res += "ZL";
  }
  if (gp & PD::Hid::Gamepad::ZR) {
    res += "ZR";
  }
  return res;
}

std::string ComboGpOut(PD::Hid::Gamepad gp) {
  std::string res = Key2String(gp);
  if (PD::Hid::IsEvent(PD::Hid::Event::Down, gp)) {
    res += ": 1";
  } else if (PD::Hid::IsEvent(PD::Hid::Event::Held, gp)) {
    res += ": 2";
  } else if (PD::Hid::IsEvent(PD::Hid::Event::Up, gp)) {
    res += ": 3";
  }
  return res;
}

class MainMenu : public PD::Ultra::Layout {
 public:
  MainMenu(PD::Li::Font& font) {
    SetFont(font);
    SetBaseViewport(PD::ivec2(1280, 720));
    pBackground.SetSize(800, 450);
    pBackground.SetColor("#ffffffff");
    pBackground.SetAlignment(UltraAlignment_Center);
    Push(pBackground);
    pText.SetColor("#ffffff");
    pText.SetText("MousePos: ");
    pText.SetAlignment(UltraAlignment_TopLeft);
    Push(pText);
    pBtn.SetText("Test");
    pBtn.SetAlignment(UltraAlignment_Center);
    pBtn.SetColor("#1273fb");
    pBtn.SetFocusedColor("#0011ff");
    pBtn.SetTextColor("#ffffff");
    pBtn.SetRounding(10);
    pBtn.OnPress([]() { PD::Log("Btn Pressed..."); });
    Push(pBtn);
  }
  ~MainMenu() {}

  void Update() {
    pText.SetText(std::format("MousePos: {}", PD::Hid::MousePos()));
  }

 private:
  PD::Ultra::Rect pBackground;
  PD::Ultra::Text pText;
  PD::Ultra::Button pBtn;
};

class App {
 public:
  App(PD::Li::Font& font) : main(font) {}
  ~App() {}

  void Update(PD::ivec2 vp, PD::Li::Drawlist& list) {
    main.Update();
    main.SetViewport(vp);
    main.Render(list);
  }

 private:
  MainMenu main;
};

struct Cursor {
  PD::fvec2 pPos = 0;
  std::string name;
  PD::Color pColor = 0xffff00ff;
  void Render(PD::Li::Drawlist& l) {
    l.DrawCircleFilled(pPos, 32.f, pColor, 15);
  }
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
  Cursor LeftStick;
  Cursor RightStick;
  RightStick.pColor = "#00ffff";
  while (pOs->Mainloop()) {
    PD::Hid::Update();
    pOs->ClearViewPort();
    PD::Li::ResetPools();  // Move to other place (or refactor this)
    app.Update(pOs->GetViewport(), pList);
    pList.SetFontscale(0.7);
    if (PD::Hid::IsEvent(PD::Hid::Event::Down, PD::Hid::Gamepad::CPLeft |
                                                   PD::Hid::Gamepad::CPRight)) {
      LeftStick.pPos.x += PD::Hid::GetLeftStick().x * 15;
    }
    if (PD::Hid::IsEvent(PD::Hid::Event::Down,
                         PD::Hid::Gamepad::CPUp | PD::Hid::Gamepad::CPDown)) {
      LeftStick.pPos.y += PD::Hid::GetLeftStick().y * 15;
    }
    if (PD::Hid::IsEvent(PD::Hid::Event::Down, PD::Hid::Gamepad::CSLeft |
                                                   PD::Hid::Gamepad::CSRight)) {
      RightStick.pPos.x += PD::Hid::GetRightStick().x * 15;
    }
    if (PD::Hid::IsEvent(PD::Hid::Event::Down,
                         PD::Hid::Gamepad::CSUp | PD::Hid::Gamepad::CSDown)) {
      RightStick.pPos.y += PD::Hid::GetRightStick().y * 15;
    }
    pList.DrawText(
        PD::fvec2(5, 37),
        std::format(
            "Input:\n  Driver: {}\n  Gamepad: {}\n    {}\n    {}\n    "
            "{}\n    {}\n    {}\n    {}\n    {}\n    {}\n    {}\n    {}\n    "
            "{}\n    {}\n    {}\n    {}\n    {}\n    {}\n    {}\n    {}\n    "
            "{}\n    {}\n    "
            "{}\n    {}\n    LS: [{}]\n    RS: [{}]\nLSP: {}",
            PD::Hid::GetDriverName(),
            bool(PD::Hid::GetFlags() & PDHidBackendFlags_HasGamepad),
            ComboGpOut(PD::Hid::Gamepad::Start),
            ComboGpOut(PD::Hid::Gamepad::Select),
            ComboGpOut(PD::Hid::Gamepad::A), ComboGpOut(PD::Hid::Gamepad::B),
            ComboGpOut(PD::Hid::Gamepad::X), ComboGpOut(PD::Hid::Gamepad::Y),
            ComboGpOut(PD::Hid::Gamepad::DDown),
            ComboGpOut(PD::Hid::Gamepad::DUp),
            ComboGpOut(PD::Hid::Gamepad::DLeft),
            ComboGpOut(PD::Hid::Gamepad::DRight),
            ComboGpOut(PD::Hid::Gamepad::L), ComboGpOut(PD::Hid::Gamepad::R),
            ComboGpOut(PD::Hid::Gamepad::ZL), ComboGpOut(PD::Hid::Gamepad::ZR),
            ComboGpOut(PD::Hid::Gamepad::CPLeft),
            ComboGpOut(PD::Hid::Gamepad::CPRight),
            ComboGpOut(PD::Hid::Gamepad::CPUp),
            ComboGpOut(PD::Hid::Gamepad::CPDown),
            ComboGpOut(PD::Hid::Gamepad::CSLeft),
            ComboGpOut(PD::Hid::Gamepad::CSRight),
            ComboGpOut(PD::Hid::Gamepad::CSUp),
            ComboGpOut(PD::Hid::Gamepad::CSDown), PD::Hid::GetLeftStick(),
            PD::Hid::GetRightStick(), LeftStick.pPos)
            .c_str(),
        "#ffffff");
    LeftStick.Render(pList);
    RightStick.Render(pList);
    pList.UnbindTexture();
    pList.PathRect(50, PD::fvec2(450, 240));
    pList.PathFillGradient("#ff0000", "#990000", PD::Radians(135));
    pList.PathAdd(PD::fvec2(100, 120));
    pList.PathAdd(PD::fvec2(250, 260));
    pList.PathAdd(PD::fvec2(420, 180));
    pList.PathAdd(PD::fvec2(600, 320));
    pList.PathAdd(PD::fvec2(820, 220));
    pList.PathAdd(PD::fvec2(1000, 360));

    pList.PathStroke("#ff00ff", 10, LiDrawFlags_AA);
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