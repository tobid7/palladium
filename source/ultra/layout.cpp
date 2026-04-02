#include <pd/lithium/formatters.hpp>
#include <pd/ultra/layout.hpp>

namespace PD {
namespace Ultra {
PD_API void Layout::SetFont(PD::Li::Font& font) { pList.SetFont(&font); }

PD_API void Layout::Render() {
  pList.Clear();
  for (auto& it : GetElements()) {
    it->Update();
    it->Draw(pList);
  }
  pList.DrawText(PD::fvec2(5, GetBotLeft().y - 40),
                 std::format("Lyt: [{}]", GetRenderspace()).c_str(),
                 0xffff00ff);
}
}  // namespace Ultra
}  // namespace PD