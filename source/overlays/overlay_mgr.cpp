#include <pd/overlays/overlay_mgr.hpp>

namespace PD {
void OverlayMgr::Push(Overlay::Ref overlay) { overlays.push_back(overlay); }
void OverlayMgr::Update(float delta) {
  for (size_t i = 0; i < overlays.size(); i++) {
    if (overlays[i]->IsKilled()) {
      overlays.erase(overlays.begin() + i);
      continue;
    }
    overlays[i]->Update(delta, ren, inp);
  }
}
}  // namespace PD