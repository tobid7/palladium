/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

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
  void* PD_INIT_DATA = nullptr;
#ifndef __3DS__
  /** Setup Window and Context */
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, 0);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif
  glfwWindowHint(GLFW_SAMPLES, 8);
  auto win = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
  glfwMakeContextCurrent(win);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  PD_INIT_DATA = win;
#else
  osSetSpeedupEnable(true);
  gfxInitDefault();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 2);
  C3D_RenderTarget* Top =
      C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTarget* Bottom =
      C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(Top, GFX_TOP, GFX_LEFT, DisplayTransferFlags);
  C3D_RenderTargetSetOutput(Bottom, GFX_BOTTOM, GFX_LEFT, DisplayTransferFlags);
#endif
  /** Init Palaldium Drivers */
  PD::Context Ctx;
#ifdef __3DS__
  Ctx.UseGfxDriver<PD::GfxC3D>(PD_INIT_DATA);
  Ctx.UseHidDriver<PD::Hid3DS>(PD_INIT_DATA);
#else
  Ctx.UseGfxDriver<PD::GfxGL2>(PD_INIT_DATA);
  Ctx.UseHidDriver<PD::HidGLFW>(PD_INIT_DATA);
#endif
  Ctx.Gfx()->Init();

  /** Create DrawList and Load Font */
  PD::Li::DrawList::Ref List = PD::Li::DrawList::New(Ctx);
  PD::Li::Font::Ref font = PD::Li::Font::New(Ctx);
#ifdef __3DS__
  font->LoadTTF("sdmc:/ComicNeue.ttf", 32);
  auto img = PD::Image::New("sdmc:/pb.png");
#else
  font->LoadTTF("ComicNeue.ttf", 32);
  auto img = PD::Image::New("pb.png");
#endif
  List->SetFont(font);
  PD::Image::Convert(img, img->RGBA);
  auto tex = Ctx.Gfx()->LoadTex(img->pBuffer, img->pWidth, img->pHeight);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  auto ui7 = PD::UI7::Context::New(Ctx);
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
  Ctx.Gfx()->SetViewPort(400, 240);
  while (aptMainLoop()) {
#endif
    Ctx.Hid()->Update();
#ifndef __3DS__
    /** Auto ViewPort Resize */
    int wx, wy;
    glfwGetWindowSize(win, &wx, &wy);
    Ctx.Gfx()->SetViewPort(wx, wy);
    ui7->GetIO()->CurrentViewPort = PD::ivec4(0, 0, wx, wy);
    glViewport(0, 0, wx, wy);
    glClearColor(0.1, 0.1, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    // ui7->pIO->GetViewPort(VpTop)->pSize = PD::ivec4(0, 0, wx, wy);
#endif
    /** Rendering some stuff */
    List->DrawSolid();
    RoundedRect(List, 10, PD::fvec2(380, 220), 0xffffffff);
    RoundedRect(List, PD::fvec2(15, 183), PD::fvec2(100, 40), 0xaa222222);
    List->DrawTexture(tex);
    RoundedRect(List, PD::fvec2(200, 50), 100, 0xffffffff,
                ((1 + std::sin(Ctx.Os()->GetTime() / 1000.f)) * 0.5f) * 100.f);
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
        "Test String oder So\nPalladium 0.6.0\n" +
            std::format("{}\nMousePos: {}",
                        PD::Strings::FormatNanos(
                            Ctx.Os()->GetTraceRef("REN")->GetLastDiff()),
                        Ctx.Hid()->MousePos()) +
            "\nUI7 Version: " + PD::UI7::GetVersion(),
        0xff000000);
    if (auto menu = ui7->BeginMenu("Test")) {
      menu->Label("Hello");
      menu->Label("World!");
      menu->Checkbox("About Menu", AboutOderSo);
      menu->DragData("Line", &TheScale);
      if (menu->Button("Test")) {
        break;
      }
      menu->Label(std::format("MousePos: {}", Ctx.Hid()->MousePos()));
      menu->Label(std::format("this->Pos: {}", menu->pLayout->GetPosition()));
      menu->Label(std::format("Dragged: #{:08X}",
                              ui7->pIO->InputHandler->DraggedObject));
      menu->Label(
          std::format("Left: {}", Ctx.Hid()->IsHeld(PD::Hid::Key::Touch)));
      menu->DragData("Value", &v, 1);
      menu->Slider("Value 2", &v);
      ui7->EndMenu();
    }
    if (auto menu = ui7->BeginMenu("Yet another Window")) {
      menu->Label(std::format("this->Pos: {}", menu->pLayout->GetPosition()));
      menu->Label(std::format("Vertices: {}", Ctx.Gfx()->VertexCounter));
      menu->Label(std::format("Indices: {}", Ctx.Gfx()->IndexCounter));
      ui7->EndMenu();
    }
    if (auto menu = ui7->BeginMenu("#Debug (UI7)")) {
      menu->Label(std::format("Framerate: {:.1f} [{:.2f}]", ui7->pIO->Framerate,
                              ui7->pIO->Delta));
      menu->SeparatorText("Input");
      menu->Label(std::format("FocusedMenu: #{:08X}",
                              ui7->pIO->InputHandler->FocusedMenu));
      menu->Label(std::format("FocusedMenuRect: {}",
                              ui7->pIO->InputHandler->FocusedMenuRect));
      menu->SeparatorText("Menu Order");
      for (auto& it : ui7->pDFO) {
        menu->Label(std::format("{}", ui7->pMenus[it]->pID.GetName()));
      }
      ui7->EndMenu();
    }
    if (auto m = ui7->BeginMenu("NoDebug")) {
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
    PD::TT::Beg(*Ctx.Os(), "ui7->Update");
    ui7->Update();
    PD::TT::End(*Ctx.Os(), "ui7->Update");
/** Render DrawData */
#ifdef __3DS__
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(Top);
    C3D_RenderTargetClear(Top, C3D_CLEAR_ALL, 0x00000000, 0);
#endif
    PD::TT::Beg(*Ctx.Os(), "REN");
    Ctx.Gfx()->NewFrame();
    Ctx.Gfx()->RenderDrawData(List->Data());
    Ctx.Gfx()->RenderDrawData(ui7->GetDrawData()->Data());
    /** Clear The List */
    List->Clear();
    PD::TT::End(*Ctx.Os(), "REN");
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

#ifndef __3DS__
int main2() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, 0);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif
  glfwWindowHint(GLFW_SAMPLES, 8);
  auto win = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
  glfwMakeContextCurrent(win);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  auto Ctx = PD::Context::Create();
  Ctx->UseOsDriver<PD::OsDriver>(nullptr);
  Ctx->UseGfxDriver<PD::GfxGL2>(nullptr);
  Ctx->UseHidDriver<PD::HidGLFW>(win);
  Ctx->Gfx()->Init();  // Need to get rid of this
  PD::Li::DrawList pDL(*Ctx);
  pDL.DrawRectFilled(0, 50, 0xffffffff);
  std::cout << Ctx->Os()->GetName() << std::endl;
  std::cout << Ctx->Gfx()->GetName() << std::endl;
  std::cout << Ctx->Hid()->GetName() << std::endl;
  while (!glfwWindowShouldClose(win)) {
    Ctx->Hid()->Update();
    int wx, wy;
    glfwGetWindowSize(win, &wx, &wy);
    Ctx->Gfx()->SetViewPort(wx, wy);
    glViewport(0, 0, wx, wy);
    glClearColor(0.1, 0.1, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    Ctx->Gfx()->NewFrame();
    Ctx->Gfx()->RenderDrawData(pDL.Data());
    glfwPollEvents();
    glfwSwapBuffers(win);
  }
  return 0;
}
#endif