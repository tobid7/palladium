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

using vec2 = PD::vec2;
using vec3 = PD::vec3;
using vec4 = PD::vec4;

class Test : public PD::App {
 public:
  Test() = default;
  ~Test() = default;

  void Init() override {
    test = PD::Texture::New("romfs:/icon.png");
    Overlays()->Push(PD::New<PD::Performance>(dbg, dbg_screen));
    font = PD::LI::Font::New();
    // font->LoadSystemFont();
    font->LoadTTF("romfs:/ComicNeue.ttf", 32);
    Renderer()->Font(font);
    ui7 = PD::UI7::Context::New();
  }

  bool MainLoop(float delta, float time) override {
    DrawFancyBG(time);
    Renderer()->OnScreen(PD::Screen::Bottom);
    Renderer()->DrawRectSolid(0, vec2(320, 240), PD::Color("#222222"));
    Renderer()->UseTex(test);
    Renderer()->Layer(Renderer()->Layer() + 1);
    Renderer()->DrawImage(
        Renderer()->GetViewport().zw() * 0.5 - test->GetSize() * 0.5, test);
    Renderer()->DrawText(5, 0xffffffff, "Hello World!", LITextFlags_None);
    if (Input()->IsDown(PD::Hid::Start)) {
      return false;
    }
    if (Input()->IsDown(Input()->A)) {
      Overlays()->Push(PD::New<PD::Performance>(dbg, dbg_screen));
      Messages()->Push("Test", "Oder SO");
      // what.To(vec2(5, 200)).From(vec2(-100,
      // 200)).In(0.5).As(what.EaseInQuad);
    }
    if (Input()->IsUp(Input()->B)) {
      Overlays()->Push(PD::New<PD::Keyboard>(text, state));
      // what.To(vec2(5, 180)).From(vec2(5, 200)).In(0.5).As(what.EaseOutQuad);
    }
    return true;
  }

  void Deinit() override {}

  void DrawFancyBG(float time) {
    Renderer()->DrawRect(vec2(0, 0), vec2(400, 240), 0xff64c9fd);
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

    Renderer()->DrawTriangle(
        vec2(x_position, y_position), vec2(x_position + 300, y_position + (90)),
        vec2(x_position - 300, y_position + (90)),
        PD::Color(.94f - .17f * color_effect, .61f - .25f * color_effect,
                  .36f + .38f * color_effect));
  }

 private:
  PD::Texture::Ref test;
  bool dbg = false, dbg_screen = false;
  std::string text;
  PD::Keyboard::State state;
  PD::UI7::Context::Ref ui7;
  PD::LI::Font::Ref font;
};

int main() {
  auto app = PD::New<Test>();
  app->Run();
  return 0;
}