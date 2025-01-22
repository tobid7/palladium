#include <pd/common/strings.hpp>
#include <pd/common/sys.hpp>
#include <pd/overlays/performance.hpp>

namespace PD {
int Performance::too = 0;

void Performance::Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) {
  if (*skill) {
    Kill();
  }
  ren->OnScreen(Screen::Top);
  ren->TextScale(0.6);
  vec2 pos;
  Line(pos, std::format("FPS {:.1f} FPS / {:.2f}ms", 1000.f / delta, delta),
       ren);
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