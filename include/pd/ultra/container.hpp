#pragma once

#include <pd/lithium/lithium.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
class Container {
 public:
  Container() {}
  Container(const PD::Li::Rect& r) : pRect(r) {}
  virtual ~Container() {}

  void Push(PD::Ultra::ElementBase* elem) {
    elem->SetParent(this);
    pElems.Push(elem);
  }
  void Reset() { pElems.ResetFast(); }
  const PD::Li::Rect& GetRenderspace() const { return pRect; }

  void SetViewport(const PD::fvec2& vp) { pRect = PD::fvec4(PD::fvec2(0), vp); }

  const PD::fvec2 GetTopLeft() const { return pRect.TopLeft(); }
  const PD::fvec2 GetTopRight() const { return pRect.TopRight(); }
  const PD::fvec2 GetBotLeft() const { return pRect.BotLeft(); }
  const PD::fvec2 GetBotRight() const { return pRect.BotRight(); }
  const PD::fvec2 GetSize() const { return pRect.BotRight() - pRect.TopLeft(); }
  const PD::fvec2 GetPosition() const { return pRect.TopLeft(); }

 protected:
  PD::Pool<PD::Ultra::ElementBase*>& GetElements() { return pElems; }

 private:
  PD::Li::Rect pRect;
  PD::Pool<PD::Ultra::ElementBase*> pElems;
};
}  // namespace Ultra
}  // namespace PD