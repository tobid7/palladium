#pragma once

#include <cstddef>
#include <pd/core/pool.hpp>
#include <pd/lithium/pools.hpp>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
class Command {
 public:
  Command() { Reset(); }
  ~Command() {}

  void Reserve(size_t vtx, size_t idx) {
    if (!FirstVertex)
      FirstVertex = AllocateVertices(vtx);
    else
      AllocateVertices(vtx);
    if (!FirstIndex)
      FirstIndex = AllocateIndices(idx);
    else
      AllocateIndices(idx);
  }

  void Reset() {
    Layer = 0;
    Tex = 0;
    FirstIndex = nullptr;
    FirstVertex = nullptr;
    IndexCount = 0;
    VertexCount = 0;
  }

  Command& Add(const Vertex& vtx) {
    FirstVertex[VertexCount++] = vtx;
    return *this;
  }
  Command& Add(u16 idx) {
    FirstIndex[IndexCount++] = VertexCount + idx;
    return *this;
  }
  Command& Add(u16 a, u16 b, u16 c) {
    FirstIndex[IndexCount++] = VertexCount + a;
    FirstIndex[IndexCount++] = VertexCount + b;
    FirstIndex[IndexCount++] = VertexCount + c;
    return *this;
  }

  int Layer = 0;
  ptr Tex = 0;
  Vertex* FirstVertex = nullptr;
  u16* FirstIndex = nullptr;
  size_t VertexCount = 0;
  size_t IndexCount = 0;
  // Todo: implement
  size_t VertexCountMax = 0;
  size_t IndexCountMax = 0;
};
}  // namespace Li
}  // namespace PD