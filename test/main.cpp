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

using vec2 = PD::vec2;
using vec3 = PD::vec3;
using vec4 = PD::vec4;

class Test : public PD::App {
 public:
  Test() = default;
  ~Test() = default;

  void Init() override {
    test = PD::Texture::New("romfs:/icon.png");
    cpu = PD::TimeStats::New(100);
  }

  bool MainLoop(unsigned long long delta, float time) override {
    cpu->Add(C3D_GetProcessingTime());
    hidScanInput();
    DrawFancyBG(time);
    Renderer()->TextScale(0.6f);
    vec2 start(5, 100);
    DebugText(start, "FPS: " + std::to_string((int)GetFps()));
    DebugText(start, "DrawCalls: " + std::to_string(Renderer()->DrawCalls()));
    DebugText(start, "DrawCommands: " + std::to_string(Renderer()->Commands()));
    DebugText(start, "Vertices: " + std::to_string(Renderer()->Vertices()));
    DebugText(start, "Indices: " + std::to_string(Renderer()->Indices()));
    DebugText(start, "Ren [AVG]: " + PD::Strings::FormatNanos(
                                         PD::Sys::GetTraceRef("LI_RenderAll")
                                             ->GetProtocol()
                                             ->GetAverage()));
    DebugText(start, "App [AVG]: " + PD::Strings::FormatNanos(
                                         PD::Sys::GetTraceRef("App_MainLoop")
                                             ->GetProtocol()
                                             ->GetAverage()));
    Renderer()->DefaultTextScale();
    Renderer()->OnScreen(PD::Screen::Bottom);
    Renderer()->DrawRectSolid(0, vec2(320, 240), PD::Color("#222222"));
    Renderer()->UseTex(test);
    Renderer()->DrawImage(vec2(130, 90), test);
    Renderer()->DrawText(5, 0xffffffff, "Hello World!", LITextFlags_None);
    if (hidKeysDown() & KEY_START) {
      return false;
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

  void DebugText(vec2& pos, const std::string& text) {
    auto tbs = Renderer()->GetTextDimensions(text);
    Renderer()->DrawRectSolid(pos, tbs, 0xaa000000);
    Renderer()->DrawText(pos, 0xffff00ff, text);
    pos[1] += tbs[1];  // Auto set new pos
  }

 private:
  PD::Texture::Ref test;
  PD::TimeStats::Ref cpu;
};

int main() {
  auto app = PD::New<Test>();
  app->Run();
  return 0;
}