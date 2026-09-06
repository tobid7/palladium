#pragma once

#include <pd/core/core.hpp>
#include <pd/lithium/rect.hpp>

namespace PD {
namespace Li {
namespace Math {
PD_API bool InBounds(const fvec2& pos, const fvec2& size, const fvec4& rect);
PD_API bool InBounds(const fvec2& pos, const fvec4& rect);
PD_API bool InBounds(const fvec2& a, const fvec2& b, const fvec2& c,
                     const fvec4& rect);
PD_API bool InSpace(const PD::fvec2& pos, const Rect& rect);
PD_API void RotateCorner(fvec2& pos, float sinus, float cosinus);
PD_API Rect PrimRect(const fvec2& pos, const fvec2& size, float angle = 0.f);
PD_API Rect PrimLine(const fvec2& a, const fvec2& b, int t = 1);
}  // namespace Math
}  // namespace Li
}  // namespace PD