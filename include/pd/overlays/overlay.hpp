#pragma once

#include <pd/common/common.hpp>
#include <pd/controls/hid.hpp>
#include <pd/graphics/lithium.hpp>

namespace PD {
class Overlay : public SmartCtor<Overlay> {
 public:
  Overlay() {}
  virtual ~Overlay() {}

  virtual void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) = 0;

  bool IsKilled() const { return kill; }

 protected:
  void Kill() { kill = true; }

 private:
  bool kill = false;
};
}  // namespace PD