#pragma once

#include <pd/core/core.hpp>
#include <pd/lithium/lithium.hpp>
#include <pd/ultra/flags.hpp>

namespace PD {
namespace Ultra {
class PD_API Canvas {
 public:
  Canvas();
  Canvas(const PD::fvec2& size);
  ~Canvas();

  void SetVirtualViewport(const PD::fvec2& size);
  void SetViewport(const PD::fvec2& size);

  PD::fvec2 VTranslatePos(const PD::fvec2& p) const;
  PD::fvec2 VTranslateAlignPos(const PD::fvec2& pos, const PD::fvec2& size,
                               UltraAlignment align) const;
  PD::Li::Rect VTranslateObject(const PD::fvec2& pos, const PD::fvec2& size,
                                UltraAlignment align,
                                bool size_modified = false) const;
  PD::Li::Rect TranslateObject(const PD::fvec2& pos, const PD::fvec2& size,
                               UltraAlignment align,
                               bool size_modified = false) const;
  PD::fvec2 TranslatePos(const PD::fvec2& p) const;
  PD::fvec2 VTranslateSize(const PD::fvec2& s) const;
  PD::fvec2 TranslateSize(const PD::fvec2& s) const;
  float VTranslateFontscale(float f) const;
  float TranslateFontscale(float f) const;
  const PD::u32& GetRevision() const;

 private:
  PD::fvec2 pViewport;
  PD::fvec2 pVirtualViewPort;
  float pVfactor = 0.f;
  PD::fvec2 pVoff;
  PD::u32 pRev;  // revision
};
}  // namespace Ultra
}  // namespace PD