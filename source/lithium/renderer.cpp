/*
MIT License

Copyright (c) 2024 - 2025 tobid7

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <pd/lithium/renderer.hpp>

namespace PD {
namespace Li {
PD_API bool Renderer::InBox(const fvec2& pos, const fvec2& szs,
                            const fvec4& rect) {
  return (pos.x + szs.x >= rect.x && pos.y + szs.y >= rect.y &&
          pos.x <= rect.z && pos.y <= rect.w);
}

PD_API bool Renderer::InBox(const fvec2& pos, const fvec4& rect) {
  return (pos.x > rect.x && pos.x < rect.x + rect.z && pos.y > rect.y &&
          pos.y < rect.y + rect.w);
}

PD_API bool Renderer::InBox(const fvec2& alpha, const fvec2& bravo,
                            const fvec2& charlie, const fvec4& rect) {
  return ((alpha.x < rect.z && bravo.x < rect.z && charlie.x < rect.z) ||
          (alpha.y < rect.w && bravo.y < rect.w && charlie.y < rect.w) ||
          (alpha.x > 0 && bravo.x > 0 && charlie.x > 0) ||
          (alpha.y > 0 && bravo.y > 0 && charlie.y > 0));
}

PD_API void Renderer::RotateCorner(fvec2& pos, float sinus, float cosinus) {
  float x = pos.x * cosinus - pos.y * sinus;
  float y = pos.y * cosinus - pos.x * sinus;
  pos = fvec2(x, y);
}

PD_API Rect Renderer::PrimRect(const fvec2& pos, const fvec2& size,
                               float angle) {
  fvec2 c = size * 0.5f;  // Center
  fvec2 corner[4] = {
      fvec2(-c.x, -c.y),
      fvec2(-c.x + size.x, -c.y),
      fvec2(-c.x, -c.y + size.y),
      fvec2(-c.x + size.x, -c.y + size.y),
  };

  // Only rotate if required
  if (angle != 0.f) {
    float s = std::sin(angle);
    float co = std::cos(angle);
    for (int i = 0; i < 4; i++) {
      RotateCorner(corner[i], s, co);
    }
  }

  // Return Result
  return Rect(corner[0] + pos + c, corner[1] + pos + c, corner[2] + pos + c,
              corner[3] + pos + c);
}

PD_API Rect Renderer::PrimLine(const fvec2& a, const fvec2& b, int thickness) {
  // Using the vec maths api makes the code as short as it is
  vec2 dir = a - b;
  float len = dir.Len();
  vec2 unit_dir = dir / len;
  vec2 perpendicular(-unit_dir.y, unit_dir.x);
  vec2 off = perpendicular * ((float)thickness * 0.5f);

  return Rect(a + off, b + off, a - off, b - off);
}

PD_API void Renderer::CmdQuad(Command::Ref cmd, const Rect& quad,
                              const Rect& uv, u32 color) {
  cmd->AddIdx(0).AddIdx(1).AddIdx(2);
  cmd->AddIdx(0).AddIdx(2).AddIdx(3);
  cmd->AddVtx(Vertex(quad.BotRight(), uv.BotRight(), color));
  cmd->AddVtx(Vertex(quad.TopRight(), uv.TopRight(), color));
  cmd->AddVtx(Vertex(quad.TopLeft(), uv.TopLeft(), color));
  cmd->AddVtx(Vertex(quad.BotLeft(), uv.BotLeft(), color));
}

PD_API void Renderer::CmdTriangle(Command::Ref cmd, const fvec2 a,
                                  const fvec2 b, const fvec2 c, u32 clr) {
  cmd->AddIdx(2).AddIdx(1).AddIdx(0);
  cmd->AddVtx(Vertex(a, vec2(0.f, 1.f), clr));
  cmd->AddVtx(Vertex(b, vec2(1.f, 1.f), clr));
  cmd->AddVtx(Vertex(c, vec2(1.f, 0.f), clr));
}

// TODO: Don't render OOS (Probably make it with a define as it
// would probably be faster to render out of screen than checking if
// it could be skipped)
PD_API void Renderer::CmdConvexPolyFilled(Command::Ref cmd,
                                          const std::vector<fvec2>& points,
                                          u32 clr, Texture::Ref tex) {
  if (points.size() < 3 || tex == nullptr) {
    return;  // Need at least three points
  }

  // Support for Custom Textures (UV calculation)
  float minX = points[0].x, minY = points[0].y;
  float maxX = minX, maxY = minY;
  // Check for the max and min Positions
  for (const auto& it : points) {
    if (it.x < minX) minX = it.x;
    if (it.y < minY) minY = it.y;
    if (it.x > maxX) maxX = it.x;
    if (it.y > maxY) maxY = it.y;
  }
  // Get Short defines for UV
  // (Bottom Right is not required)
  auto uv_tl = tex->UV.TopLeft();
  auto uv_tr = tex->UV.TopRight();
  auto uv_bl = tex->UV.BotLeft();

  // Render
  for (int i = 2; i < (int)points.size(); i++) {
    cmd->AddIdx(0).AddIdx(i).AddIdx(i - 1);
  }
  for (int i = 0; i < (int)points.size(); i++) {
    // Calculate U and V coords
    float u =
        uv_tl.x + ((points[i].x - minX) / (maxX - minX)) * (uv_tr.x - uv_tl.x);
    float v =
        uv_tl.y + ((points[i].y - minY) / (maxY - minY)) * (uv_bl.y - uv_tl.y);
    cmd->AddVtx(Vertex(points[i], fvec2(u, v), clr));
  }
}
}  // namespace Li
}  // namespace PD