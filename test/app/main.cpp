/*
MIT License
Copyright (c) 2024 René Amthor (tobid7)

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

#include <3ds.h>

#include <ctime>
#include <pd.hpp>
#include <pd/maths/tween.hpp>

class Test : public PD::App {
 public:
  Test() = default;
  ~Test() = default;

  void Init() override {
    ren = Renderer();
    inp = Input();
    test = PD::Texture::New("romfs:/icon.png");
    // Performance Overlay freezes N3DS
    // Overlays()->Push(PD::New<PD::Performance>(dbg, dbg_screen));
    font = PD::LI::Font::New();
    // font->LoadTTF("romfs:/fonts/ComicNeue.ttf", 32);
    font->LoadTTF("romfs:/fonts/JetBrainsMono-Medium.ttf", 32);
    ren->Font(font);
    ui7 = PD::UI7::Context::New(ren, inp);
  }

  bool MainLoop(float delta, float time) override {
    ren->OnScreen(Top);
    DrawFancyBG(time);
    ren->OnScreen(Bottom);
    if (ui7->BeginMenu("Test",
                       UI7MenuFlags_Scrolling | UI7MenuFlags_CenterTitle)) {
      auto m = ui7->GetCurrentMenu();
      m->SeparatorText("Menu Timings");
      m->DebugLabels();
      m->SeparatorText("Palladium Info");
      m->Label("Version: " + PD::LibInfo::Version() + " [" +
               PD::LibInfo::Commit() + "]");
      m->AfterAlignCenter();
      m->Label("CompileInfo: " + PD::LibInfo::CompiledWith() + " - " +
               PD::LibInfo::CxxVersion());
      m->AfterAlignCenter();
      m->Label("Build at " + PD::LibInfo::BuildTime());
      m->AfterAlignCenter();
      m->SeparatorText("Basic Info");
      m->Label("sizeof(size_t): " + std::to_string(sizeof(size_t)) + " -> " +
               std::to_string(sizeof(size_t) * 8) + "Bit");
      m->AfterAlignCenter();
      m->Label("__cplusplus=" + std::to_string(__cplusplus));
      m->AfterAlignCenter();
      m->Label(PD::Strings::GetCompilerVersion());
      m->AfterAlignCenter();
      m->Label("sizeof(LI::Vertex): " + std::to_string(sizeof(PD::LI::Vertex)));
      m->AfterAlignCenter();
      m->Label("sizeof(PD::u16): " + std::to_string(sizeof(PD::u16)));
      m->AfterAlignCenter();
      m->SeparatorText("UI7 Tests");
      m->Label("This seems to be a label");
      m->Image(test);
      m->Separator();
      if (m->Button("Button?")) {
        Messages()->Push("Button", "Pressed...");
      }
      m->SeparatorText("SeparatorText");
      m->Checkbox("Test", cbtest);
      ui7->EndMenu();
    }
    ui7->Update(delta);
    if (inp->IsDown(PD::Hid::Start)) {
      return false;
    }
    if (inp->IsDown(inp->A)) {
      Overlays()->Push(PD::New<PD::Performance>(dbg, dbg_screen));
      Messages()->Push("Test", "Oder SO");
    }
    if (inp->IsUp(inp->B)) {
      Overlays()->Push(PD::New<PD::Keyboard>(text, state));
    }
    return true;
  }

  void Deinit() override {}

 private:
  /// Shorter Acess to Renderer / Input
  PD::LI::Renderer::Ref ren;
  PD::Hid::Ref inp;
  /// Other Data
  PD::Texture::Ref test;
  bool dbg = false, dbg_screen = false;
  bool cbtest = true;
  std::string text;
  PD::Keyboard::State state;
  PD::UI7::Context::Ref ui7;
  PD::LI::Font::Ref font;
  vec2 text_pos;
};

int main() {
  Test app;
  app.Run();
  return 0;
}