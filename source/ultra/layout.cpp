#include <pd/ultra/layout.hpp>

namespace PD {
namespace Ultra {
PD_API void Layout::Add(ElementBase& elem) { *pElements.Allocate() = &elem; }

PD_API void Layout::SetFont(PD::Li::Font& font) { pList.SetFont(&font); }

PD_API void Layout::Render() {
  pList.Clear();
  for (auto& it : pElements) {
    it->Update(pCanvas);
    it->Draw(pList);
  }
}
}  // namespace Ultra
}  // namespace PD