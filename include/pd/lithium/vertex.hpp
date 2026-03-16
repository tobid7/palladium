#pragma once

#include <pd/core/vec.hpp>

namespace PD {
namespace Li {
class Vertex {
 public:
  Vertex() {}
  Vertex(const fvec2& pos, const fvec2& uv, u32 color)
      : pos(pos), uv(uv), color(color) {}
  ~Vertex() {}

  fvec2 pos;
  fvec2 uv;
  u32 color = 0x00000000;
};
}  // namespace Li
}  // namespace PD