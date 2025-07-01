#include <palladium>
#ifdef __3DS__
#include <pd-3ds.hpp>
const u32 DisplayTransferFlags =
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
     GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
     GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));
#else
#include <pd-desktop.hpp>
#endif

void RoundedRect(PD::Li::DrawList::Ref l, PD::fvec2 p, PD::fvec2 s, PD::u32 clr,
                 float r = 10.f) {
  l->PathRect(p, p + s, r);
  l->PathFill(clr);
}

int v = 20;
int TheScale = 1;
bool AboutOderSo = true;

int main() {
  void *PD_INIT_DATA = nullptr;
#ifndef __3DS__
  /** Setup Window and Context */
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_SAMPLES, 8);
  auto win = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
  glfwMakeContextCurrent(win);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  PD_INIT_DATA = win;
#else
  osSetSpeedupEnable(true);
  gfxInitDefault();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 2);
  C3D_RenderTarget *Top =
      C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTarget *Bottom =
      C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(Top, GFX_TOP, GFX_LEFT, DisplayTransferFlags);
  C3D_RenderTargetSetOutput(Bottom, GFX_BOTTOM, GFX_LEFT, DisplayTransferFlags);
#endif
  /** Init Palaldium Drivers */
  PD::Init(PD_INIT_DATA);
  /** Create DrawList and Load Font */
  PD::Li::DrawList::Ref List = PD::Li::DrawList::New();
  PD::Li::Font::Ref font = PD::Li::Font::New();
#ifdef __3DS__
  font->LoadTTF("sdmc:/ComicNeue.ttf", 32);
  auto img = PD::Image::New("sdmc:/pb.png");
#else
  font->LoadTTF("ComicNeue.ttf", 32);
  auto img = PD::Image::New("pb.png");
#endif
  List->SetFont(font);
  PD::Image::Convert(img, img->RGBA);
  auto tex = PD::Li::Gfx::LoadTex(img->pBuffer, img->pWidth, img->pHeight);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  auto ui7 = PD::UI7::Context::New();
  PD::UI7::ID VpTop("Default");
  ui7->AddViewPort(VpTop, PD::ivec4(0, 0, 400, 240));
  ui7->UseViewPort(VpTop);
  ui7->pIO->Font = font;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** MainLoop */
#ifndef __3DS__
  font->DefaultPixelHeight = 32;
  while (!glfwWindowShouldClose(win)) {
#else
  PD::Li::Gfx::pGfx->ViewPort = PD::ivec2(400, 240);
  while (aptMainLoop()) {
#endif
    PD::Hid::Update();
#ifndef __3DS__
    /** Auto ViewPort Resize */
    int wx, wy;
    glfwGetWindowSize(win, &wx, &wy);
    PD::Li::Gfx::pGfx->ViewPort = PD::ivec2(wx, wy);
    // ui7->pIO->GetViewPort(VpTop)->pSize = PD::ivec4(0, 0, wx, wy);
#endif
    /** Rendering some stuff */
    List->DrawSolid();
    RoundedRect(List, 10, PD::fvec2(380, 220), 0xffffffff);
    RoundedRect(List, PD::fvec2(15, 183), PD::fvec2(100, 40), 0xaa222222);
    List->DrawTexture(tex);
    RoundedRect(List, PD::fvec2(200, 50), 100, 0xffffffff,
                ((1 + std::sin(PD::OS::GetTime() / 1000.f)) * 0.5f) * 100.f);
    List->DrawText(PD::fvec2(50, 190), "OK", 0xffffffff);
    // List->DrawLine(PD::fvec2(0), PD::fvec2(1000, 600), 0xffffffff);
    List->PathAdd(500);
    List->PathAdd(550);
    List->PathStroke(0xff00ffff, TheScale);
    // List->DrawRectFilled(PD::fvec2(10, 10), PD::fvec2(1260, 700),
    // 0xffffffff);
    /** Draw text */
    List->DrawText(
        20,
        "Test String oder So\nPalladium 0.5.0\n" +
            std::format("{}\nMousePos: {}",
                        PD::Strings::FormatNanos(
                            PD::OS::GetTraceRef("REN")->GetLastDiff()),
                        PD::Hid::MousePos()) +
            "\nUI7 Version: " + PD::UI7::GetVersion(),
        0xff000000);
    if (ui7->BeginMenu("Test")) {
      auto menu = ui7->pCurrent;
      menu->Label("Hello");
      menu->Label("World!");
      menu->Checkbox("About Menu", AboutOderSo);
      menu->DragData("Line", &TheScale);
      if (menu->Button("Test")) {
        break;
      }
      menu->Label(std::format("MousePos: {}", PD::Hid::MousePos()));
      menu->Label(std::format("this->Pos: {}", menu->pLayout->GetPosition()));
      menu->Label(std::format("Dragged; #{:08X}",
                              ui7->pIO->InputHandler->DraggedObject));
      menu->Label(
          std::format("Left: {}", PD::Hid::IsHeld(PD::Hid::Key::Touch)));
      menu->DragData("Value", &v, 1);
      ui7->EndMenu();
    }
    if (ui7->BeginMenu("Yet another Window")) {
      auto menu = ui7->pCurrent;
      menu->Label(std::format("this->Pos: {}", menu->pLayout->GetPosition()));
      menu->Label(
          std::format("Vertices: {}", PD::Li::Gfx::pGfx->VertexCounter));
      menu->Label(std::format("Indices: {}", PD::Li::Gfx::pGfx->IndexCounter));
      ui7->EndMenu();
    }
    if (ui7->BeginMenu("#Debug (UI7)")) {
      auto menu = ui7->pCurrent;
      menu->Label(std::format("Framerate: {:.1f} [{:.2f}]", ui7->pIO->Framerate,
                              ui7->pIO->Delta));
      menu->SeparatorText("Input");
      menu->Label(std::format("FocusedMenu: #{:08X}",
                              ui7->pIO->InputHandler->FocusedMenu));
      menu->Label(std::format("FocusedMenuRect: {}",
                              ui7->pIO->InputHandler->FocusedMenuRect));
      menu->SeparatorText("Menu Order");
      for (auto &it : ui7->pDFO) {
        menu->Label(std::format("{}", ui7->pMenus[it]->pID.GetName()));
      }
      ui7->EndMenu();
    }
    if (ui7->BeginMenu("NoDebug")) {
      auto m = ui7->pCurrent;
      if (m->BeginTreeNode("Test")) {
        m->Label("Hello World!");
        if (m->BeginTreeNode("AnotherNode")) {
          m->Label("Another Label!");
          m->EndTreeNode();
        }
        m->EndTreeNode();
      }
      m->Label("Yes another Label!");
      ui7->EndMenu();
    }
    ui7->AboutMenu(&AboutOderSo);
    ui7->MetricsMenu();
    ui7->StyleEditor();
    PD::TT::Beg("ui7->Update");
    ui7->Update();
    PD::TT::End("ui7->Update");
/** Render DrawData */
#ifdef __3DS__
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(Top);
    C3D_RenderTargetClear(Top, C3D_CLEAR_ALL, 0x00000000, 0);
#endif
    PD::TT::Beg("REN");
    PD::Li::Gfx::NewFrame();
    PD::Li::Gfx::RenderDrawData(List->pDrawList);
    PD::Li::Gfx::RenderDrawData(ui7->GetDrawData()->pDrawList);
    /** Clear The List */
    List->Clear();
    PD::TT::End("REN");
#ifndef __3DS__
    /** Do OS Specifc Stuff (swapp buffers / window buttan events) */
    glfwPollEvents();
    glfwSwapBuffers(win);
#else
    C3D_FrameEnd(0);
#endif
  }
#ifndef __3DS__
  glfwTerminate();
#endif
  return 0;
}
