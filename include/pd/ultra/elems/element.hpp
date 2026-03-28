#pragma once

#include <pd/lithium/lithium.hpp>
#include <pd/ultra/flags.hpp>

namespace PD {
namespace Ultra {
class Canvas;
class PD_API ElementBase {
 public:
  ElementBase() {}
  ~ElementBase() {}

  virtual void Draw(PD::Li::Drawlist& dl) = 0;
  /**
   * Reset Function (for PD::Pool::FastReset)
   */
  virtual void Reset() {}
  void Update(Canvas& c);

  void SetAlignment(UltraAlignment a) { pAlignment = a; }
  void SetPosition(const PD::fvec2& pos) { pPos = pos; }
  void SetPosition(float x, float y) { pPos = PD::fvec2(x, y); }
  void SetSize(const PD::fvec2& size) { pSize = size; }
  void SetSize(float w, float h) { pSize = PD::fvec2(w, h); }

 protected:
  bool RevisionUpdate(PD::u32 req);
  PD::u32 pCanvasRev = 0;
  PD::Li::Rect pRenderspace;
  UltraAlignment pAlignment = 0;
  PD::fvec2 pPos;
  PD::fvec2 pSize;
  ElementBase* pParent = nullptr;
};
}  // namespace Ultra
}  // namespace PD