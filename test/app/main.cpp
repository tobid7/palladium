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
    DrawFancyBG(time);
    ren->OnScreen(PD::Screen::Bottom);
    // ren->DrawRectSolid(0, vec2(320, 240), PD::Color("#222222"));
    // ren->Layer(ren->Layer() + 1);
    // ren->DrawImage(ren->GetViewport().zw() * 0.5 - test->GetSize() * 0.5,
    // test); ren->DrawText(5, 0xffffffff, "Hello World!", LITextFlags_None);
    if (ui7->BeginMenu("Test",
                       UI7MenuFlags_Scrolling | UI7MenuFlags_CenterTitle)) {
      auto m = ui7->GetCurrentMenu();
      m->SeparatorText("Menu Timings");
      m->DebugLabels();
      m->SeparatorText("Lithium Settings");
      FlagBox(m, "LI AST", PD::LI::RenderFlags_AST);
      FlagBox(m, "LI LRS", PD::LI::RenderFlags_LRS);
      FlagBox(m, "LI TMS", PD::LI::RenderFlags_TMS);
      m->SeparatorText("UI7 Tests");
      m->Label("This seems to be a label");
      m->Separator();
      m->Button("Button?");
      m->SeparatorText("SeparatorText");
      m->Checkbox("Test", cbtest);
      for (int i = 0; i < 10; i++) {
        m->Label("Label: " + std::to_string(i));
      }
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

  void FlagBox(PD::UI7::Menu::Ref m, const std::string& label,
               PD::LI::RenderFlags flag) {
    bool has_flag = ren->GetFlags() & flag;
    m->Checkbox(label, has_flag);
    if (has_flag != (ren->GetFlags() & flag)) {
      if (has_flag) {
        ren->GetFlags() |= flag;
      } else {
        ren->GetFlags() &= ~flag;
      }
    }
  }

  void DrawFancyBG(float time) {
    ren->DrawRect(vec2(0, 0), vec2(400, 240), 0xff64c9fd);
    for (int i = 0; i < 44; i++) Append(i, vec2(0, 0), vec2(400, 240), time);
  }

  float Offset(float x) {
    float y = cos(x) * 42;
    return y - floor(y);
  }
  void Append(int index, vec2 position, vec2 size, float time) {
    float offset = Offset(index) * 62;
    float x_position = position.x() + size.x() / 8 * ((index % 11) - 1) +
                       cos(offset + time) * 10;
    float y_position = position.y() + size.y() / 8 * (index / 11) + 40 +
                       sin(offset + time) * 10 + 30;
    float color_effect = 1 - exp(-(index / 11) / 3.0f);

    ren->DrawTriangle(
        vec2(x_position, y_position), vec2(x_position + 300, y_position + (90)),
        vec2(x_position - 300, y_position + (90)),
        PD::Color(.94f - .17f * color_effect, .61f - .25f * color_effect,
                  .36f + .38f * color_effect));
  }

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