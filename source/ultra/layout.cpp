#include <pd/lithium/formatters.hpp>
#include <pd/ultra/layout.hpp>

namespace PD {
namespace Ultra {
PD_API void Layout::Render(PD::Li::Drawlist& list) {
  float fc = list.GetFontScale();
  list.SetFontscale(GetCanvas().VTranslateFontscale(fc));
  for (auto& it : GetElements()) {
    it->SetFontIfNull(*pFont);
    it->Update();
    it->Draw(list);
  }
  list.DrawText(PD::fvec2(5, GetBotLeft().y - 40),
                std::format("Lyt: [{}]", GetRenderspace()).c_str(), 0xffff00ff);
  list.SetFontscale(fc);
}
}  // namespace Ultra
}  // namespace PD