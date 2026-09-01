#pragma once

#include <pd/core/vec.hpp>

namespace PD {
namespace Li {
class Vertex {
 public:
  Vertex() {}
  Vertex(const fvec2& pos, const fvec2& uv, u32 color)
      : pos(pos), uv(uv), color(color) {}
  Vertex(float x, float y, float u, float v, u32 color)
      : pos(x, y), uv(u, v), color(color) {}
  ~Vertex() {}

  void Reset() {
    pos = fvec2(0.f);
    uv = fvec2(0.f);
    color = 0x00000000;
  }

  fvec2 pos;
  fvec2 uv;
  u32 color = 0x00000000;
};
}  // namespace Li
}  // namespace PD