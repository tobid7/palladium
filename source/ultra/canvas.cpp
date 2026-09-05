#include <pd/ultra/canvas.hpp>

namespace PD {
namespace Ultra {
PD_API Canvas::Canvas() { pRev = 0; }

PD_API Canvas::Canvas(const PD::fvec2& size) : pViewport(size) { pRev = 0; }

PD_API Canvas::~Canvas() {}

PD_API void Canvas::SetVirtualViewport(const PD::fvec2& size) {
  pVirtualViewPort = size;
  if (pVirtualViewPort.x && pVirtualViewPort.y) {
    pVfactor = std::min(pViewport.x / pVirtualViewPort.x,
                        pViewport.y / pVirtualViewPort.y);
    pVoff = (pViewport - (pVirtualViewPort * pVfactor)) * 0.5f;
  }
  pRev++;
}

PD_API void Canvas::SetViewport(const PD::fvec2& size) {
  if (pViewport == size) return;
  pViewport = size;
  SetVirtualViewport(pVirtualViewPort);  // recalculate Vfactor
}

PD_API PD::fvec2 Canvas::VTranslatePos(const PD::fvec2& p) const {
  return p * pVfactor + pVoff;
}

PD_API PD::fvec2 Canvas::TranslatePos(const PD::fvec2& p) const {
  return p * pViewport;
}

PD_API PD::fvec2 Canvas::VTranslateSize(const PD::fvec2& s) const {
  return s * pVfactor;
}

PD_API PD::fvec2 Canvas::TranslateSize(const PD::fvec2& s) const {
  return s * pViewport.y;
}

PD_API float Canvas::VTranslateFontscale(float f) const { return f * pVfactor; }

PD_API float Canvas::TranslateFontscale(float f) const {
  return f * pViewport.y;
}

PD_API PD::Li::Rect Canvas::VTranslateObject(const PD::fvec2& pos,
                                             const PD::fvec2& size,
                                             UltraAlignment align,
                                             bool size_modified) const {
  PD::fvec2 nsize = size;
  if (!size_modified) nsize *= pVfactor;
  PD::fvec2 final;
  if (align & UltraAlignment_Left) {
    final.x = pos.x * pVfactor;
  } else if (align & UltraAlignment_Right) {
    final.x = pViewport.x - (pos.x * pVfactor) - nsize.x;
  } else if (align & UltraAlignment_CenterHorizontal) {
    final.x = pViewport.x * 0.5 - nsize.x * 0.5 + pos.x * pVfactor;
  } else {
    final.x = pVoff.x + (pos.x * pVfactor) - nsize.x * 0.5;
  }
  if (align & UltraAlignment_Top) {
    final.y = pos.y * pVfactor;
  } else if (align & UltraAlignment_Bot) {
    final.y = pViewport.y - (pos.y * pVfactor) - nsize.y;
  } else if (align & UltraAlignment_CenterVertical) {
    final.y = pViewport.y * 0.5 - nsize.y * 0.5 + pos.y * pVfactor;
  } else {
    final.y = pVoff.y + (pos.y * pVfactor) - nsize.y * 0.5;
  }
  return PD::fvec4(final, final + nsize);
}

PD_API PD::Li::Rect Canvas::TranslateObject(const PD::fvec2& pos,
                                            const PD::fvec2& size,
                                            UltraAlignment align,
                                            bool size_modified) const {
  PD::fvec2 nsize = size;
  if (!size_modified) nsize *= pViewport;
  PD::fvec2 final;
  if (align & UltraAlignment_Left) {
    final.x = pos.x * pViewport.x;
  } else if (align & UltraAlignment_Right) {
    final.x = pViewport.x - (pos.x * pViewport.x) - nsize.x;
  } else if (align & UltraAlignment_CenterHorizontal) {
    final.x = pViewport.x * 0.5 - nsize.x * 0.5 + pos.x * pViewport.x;
  } else {
    final.x = pVoff.x + (pos.x * pViewport.x) - nsize.x * 0.5;
  }
  if (align & UltraAlignment_Top) {
    final.y = pos.y * pViewport.y;
  } else if (align & UltraAlignment_Bot) {
    final.y = pViewport.y - (pos.y * pViewport.y) - nsize.y;
  } else if (align & UltraAlignment_CenterVertical) {
    final.y = pViewport.y * 0.5 - nsize.y * 0.5 + pos.y * pViewport.y;
  } else {
    final.y = pVoff.y + (pos.y * pViewport.y) - nsize.y * 0.5;
  }
  return PD::fvec4(final, final + nsize);
}

PD_API PD::fvec2 Canvas::VTranslateAlignPos(const PD::fvec2& pos,
                                            const PD::fvec2& size,
                                            UltraAlignment align) const {
  return VTranslateObject(pos, size, align).TopLeft();
}

PD_API const PD::u32& Canvas::GetRevision() const { return pRev; }

}  // namespace Ultra
}  // namespace PD
