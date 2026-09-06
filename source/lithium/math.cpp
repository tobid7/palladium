#include <pd/lithium/math.hpp>

namespace PD {
namespace Li {
namespace Math {
PD_API bool InBounds(const fvec2& pos, const fvec2& size, const fvec4& rect) {
  return (pos.x + size.x >= rect.x && pos.y + size.y >= rect.y &&
          pos.x <= rect.z && pos.y <= rect.w);
}

PD_API bool InBounds(const fvec2& pos, const fvec4& rect) {
  return (pos.x > rect.x && pos.x < rect.x + rect.z && pos.y > rect.y &&
          pos.y < rect.y + rect.w);
}

PD_API bool InBounds(const fvec2& a, const fvec2& b, const fvec2& c,
                     const fvec4& rect) {
  return ((a.x < rect.z && b.x < rect.z && c.x < rect.z) ||
          (a.y < rect.w && b.y < rect.w && c.y < rect.w) ||
          (a.x > 0 && b.x > 0 && c.x > 0) || (a.y > 0 && b.y > 0 && c.y > 0));
}

PD_API void RotateCorner(fvec2& pos, float sinus, float cosinus) {
  float x = pos.x * cosinus - pos.y * sinus;
  float y = pos.y * cosinus - pos.x * sinus;
  pos = fvec2(x, y);
}

PD_API Rect PrimRect(const fvec2& pos, const fvec2& size, float a) {
  fvec2 c = size * 0.5f;  // Center
  fvec2 corner[4] = {
      fvec2(-c.x, -c.y),
      fvec2(-c.x + size.x, -c.y),
      fvec2(-c.x, -c.y + size.y),
      fvec2(-c.x + size.x, -c.y + size.y),
  };

  // Only rotate if required
  if (a != 0.f) {
    float s = std::sin(a);
    float co = std::cos(a);
    for (int i = 0; i < 4; i++) {
      RotateCorner(corner[i], s, co);
    }
  }

  // Return Result
  return Rect(corner[0] + pos + c, corner[1] + pos + c, corner[2] + pos + c,
              corner[3] + pos + c);
}

PD_API Rect PrimLine(const fvec2& a, const fvec2& b, int t) {
  // Using the vec maths api makes the code as short as it is
  vec2 dir = a - b;
  float len = dir.Len();
  vec2 unit_dir = dir / len;
  vec2 perpendicular(-unit_dir.y, unit_dir.x);
  vec2 off = perpendicular * ((float)t * 0.5f);

  return Rect(a + off, b + off, a - off, b - off);
}

}  // namespace Math
}  // namespace Li
}  // namespace PD