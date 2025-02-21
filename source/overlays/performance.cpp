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

#include <pd/core/strings.hpp>
#include <pd/core/sys.hpp>
#include <pd/overlays/performance.hpp>

namespace PD {
int Performance::too = 0;

void Performance::Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) {
  if (*skill) {
    Kill();
  }
  ren->OnScreen(ren->GetScreen(*screen));
  ren->TextScale(0.6);
  vec2 pos;
  Line(pos, std::format("{:.1f} FPS / {:.2f}ms", 1000.f / delta, delta), ren);
  Line(pos, "Ren [AVG]: " + TSA("LI_RenderAll"), ren);
  Line(pos, "App [AVG]: " + TSA("App_MainLoop"), ren);
  Line(pos, "Ovl [AVG]: " + TSA("Ovl_Update"), ren);
  Line(pos,
       "VI: [" + std::to_string(ren->Vertices()) + ", " +
           std::to_string(ren->Indices()) + "]",
       ren);
  Line(pos,
       "DC: [" + std::to_string(ren->DrawCalls()) + ", " +
           std::to_string(ren->Commands()) + "]",
       ren);
  Line(pos, "AST: " + std::to_string(ren->AstUsage()), ren);
  Line(pos, "TMS: " + std::to_string(ren->TmsUsage()), ren);
  ren->DefaultTextScale();
}

void Performance::Line(vec2& pos, const std::string& text,
                       LI::Renderer::Ref ren) {
  auto tbs = ren->GetTextDimensions(text);
  int l = ren->Layer();
  ren->DrawRectSolid(pos, tbs, 0xaa000000);
  ren->Layer(l + 1);
  ren->DrawText(pos, 0xffff00ff, text);
  ren->Layer(l);
  pos[1] += tbs[1];  // Auto set new pos
}

std::string Performance::TSA(const std::string& id) {
  return PD::Strings::FormatNanos(
      PD::Sys::GetTraceRef(id)->GetProtocol()->GetAverage());
}
}  // namespace PD