#pragma once

#include <pd/lithium/lithium.hpp>
#include <pd/ultra/flags.hpp>

namespace PD {
namespace Ultra {
class Canvas;
class Container;
class PD_API ElementBase {
 public:
  ElementBase() {}
  ~ElementBase() {}

  virtual void Draw(PD::Li::Drawlist& dl) = 0;
  /**
   * Reset Function (for PD::Pool::FastReset)
   */
  virtual void Reset() {}

  void SetAlignment(UltraAlignment a) { pAlignment = a; }
  void SetPosition(const PD::fvec2& pos) { pPos = pos; }
  void SetPosition(float x, float y) { pPos = PD::fvec2(x, y); }
  void SetSize(const PD::fvec2& size) { pSize = size; }
  void SetSize(float w, float h) { pSize = PD::fvec2(w, h); }

 protected:
  friend class Container;
  void SetParent(Container* c) { pParent = c; }
  bool RevisionUpdate(PD::u32 req);
  Container* pParent;
  PD::u32 pCanvasRev = 0;
  UltraAlignment pAlignment = 0;
  PD::fvec2 pPos;
  PD::fvec2 pSize;
};
}  // namespace Ultra
}  // namespace PD