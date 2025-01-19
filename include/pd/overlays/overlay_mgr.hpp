#pragma once

#include <pd/controls/hid.hpp>
#include <pd/graphics/lithium.hpp>
#include <pd/overlays/overlay.hpp>

namespace PD {
class OverlayMgr : public SmartCtor<OverlayMgr> {
 public:
  OverlayMgr(LI::Renderer::Ref ren, Hid::Ref inp) {
    this->ren = ren;
    this->inp = inp;
  }
  ~OverlayMgr() { overlays.clear(); }

  void Push(Overlay::Ref overlay);
  void Update(float delta);

 private:
  std::vector<Overlay::Ref> overlays;
  LI::Renderer::Ref ren;
  Hid::Ref inp;
};
}  // namespace PD