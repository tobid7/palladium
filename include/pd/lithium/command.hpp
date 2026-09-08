#pragma once

#include <cstddef>
#include <pd/core/pool.hpp>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
class Command {
 public:
  Command() { Reset(); }
  ~Command() {}

  void Reserve(size_t vtx, size_t idx);
  void Reset();

  Command& Add(const Vertex& vtx);
  Command& Add(u16 idx);
  Command& Add(u16 a, u16 b, u16 c);

  int Layer = 0;
  ptr Tex = 0;
  bool SDF = false;
  size_t FirstVertex = 0;
  size_t FirstIndex = 0;
  size_t VertexCount = 0;
  size_t IndexCount = 0;
  // Todo: implement
  size_t VertexCountMax = 0;
  size_t IndexCountMax = 0;
  // ClipRect
  PD::fvec4 ClipRect;
  bool ClipRectUsed = false;
};
}  // namespace Li
}  // namespace PD