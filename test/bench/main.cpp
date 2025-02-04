/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

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

#include <pd.hpp>

struct StatsRes {
  std::string name;
  std::string average;
  std::string min;
  std::string max;
};

class TestBench : public PD::App {
 public:
  TestBench() = default;
  ~TestBench() = default;

  void Init() override {
    /// Test in OLD3DS Mode
    osSetSpeedupEnable(false);
    PD::TT::Beg("BaseInit");
    inp = Input();
    ren = Renderer();
    font = PD::LI::Font::New();
    font->LoadTTF("romfs:/fonts/JetBrainsMono-Medium.ttf", 32);
    ren->Font(font);
    ui7 = PD::UI7::Context::New(ren, inp);
    /// Maximum frames for 5 seconds
    li_stats = PD::TimeStats::New(300);
    PD::TT::End("BaseInit");
  }

  bool MainLoop(float delta, float time) {
    if (stime == 0.f) {
      stime = time;
    }
    switch (test) {
      case 0:
        Test1(time);
        break;
      case 1:
        Test2(delta, time);
        break;
      default:
        Result(delta);
    }
    if (time - stime > 5.f && test < 2) {
      if (test == 0) {
        results.push_back(std::make_pair<std::string, std::vector<StatsRes>>(
            std::to_string(test + 1),
            {MakeRes("LI", li_stats),
             MakeRes("Scene1", PD::Sys::GetTraceRef("Test1")->GetProtocol())}));
      } else if (test == 1) {
        results.push_back(std::make_pair<std::string, std::vector<StatsRes>>(
            std::to_string(test + 1),
            {MakeRes("LI", li_stats),
             MakeRes("Scene2", PD::Sys::GetTraceRef("Test2")->GetProtocol())}));
      } else {
        results.push_back(std::make_pair<std::string, std::vector<StatsRes>>(
            std::to_string(test + 1), {MakeRes("LI", li_stats)}));
      }
      test++;
      stime = time;
      frame = 0;
      li_stats->Clear();
    }
    frame++;
    return true;
  }

  StatsRes MakeRes(const std::string& name, PD::TimeStats::Ref s) {
    StatsRes res;
    res.name = name;
    res.average = PD::Strings::FormatNanos(s->GetAverage());
    res.min = PD::Strings::FormatNanos(s->GetMin());
    res.max = PD::Strings::FormatNanos(s->GetMax());
    return res;
  }

  void Result(float delta) {
    UpdateLiTimes();
    ren->OnScreen(Top);
    if (ui7->BeginMenu("TestBench")) {
      auto m = ui7->GetCurrentMenu();
      m->Label("Base Init: " + TTime("BaseInit"));
      m->Label("Render All: " +
               PD::Strings::FormatNanos(li_stats->GetAverage()));
      ui7->EndMenu();
    }
    ren->OnScreen(Bottom);
    if (ui7->BeginMenu("Test Results", UI7MenuFlags_Scrolling)) {
      auto m = ui7->GetCurrentMenu();
      for (auto& it : results) {
        m->SeparatorText("Test " + it.first);
        for (int i = 0; i < (int)it.second.size(); i++) {
          m->Label(it.second[i].name + ":");
          m->Label("AVG: " + it.second[i].average);
          m->Label("MIN: " + it.second[i].min);
          m->Label("MAX: " + it.second[i].max);
          if (i != (int)it.second.size() - 1) {
            m->Separator();
          }
        }
      }
      ui7->EndMenu();
    }
    ui7->Update(delta);
  }

  void Test1(float time) {
    PD::TT::Scope st("Test1");
    UpdateLiTimes();
    DrawFancyBG(time);
  }

  void Test2(float delta, float time) {
    PD::TT::Scope st("Test2");
    UpdateLiTimes();
    DrawFancyBG(time);
    if (ui7->BeginMenu("Test2")) {
      auto m = ui7->GetCurrentMenu();
      m->Label("Text1");
      m->Label("Text2");
      m->Button("Test");
      m->SameLine();
      m->Button("Test2");
      m->Separator();
      m->Label("Line1");
      m->Label("Line2");
      ui7->EndMenu();
    }
    ui7->Update(delta);
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

  std::string TTime(const std::string& id) {
    return PD::Strings::FormatNanos(PD::Sys::GetTraceRef(id)->GetLastDiff());
  }

  void UpdateLiTimes() {
    if (frame) {
      li_stats->Add(PD::Sys::GetTraceRef("LI_RenderAll")->GetLastDiff());
    }
  }

 private:
  PD::Hid::Ref inp;
  PD::LI::Renderer::Ref ren;
  PD::UI7::Context::Ref ui7;
  PD::LI::Font::Ref font;
  PD::TimeStats::Ref li_stats;
  int test = 0;
  float stime = 0.f;
  std::vector<std::pair<std::string, std::vector<StatsRes>>> results;
  int frame = 0;
};

int main() {
  TestBench app;
  app.Run();
  return 0;
}