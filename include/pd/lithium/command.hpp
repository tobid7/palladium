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
    if (!FirstVertex) {
      FirstVertex = AllocateVertices(vtx, (PD::ptr)this);
      VertexCountMax = vtx;
    } else {
      ExpandVertices(vtx, (PD::ptr)this);
      VertexCountMax += vtx;
    }
    if (!FirstIndex) {
      FirstIndex = AllocateIndices(idx, (PD::ptr)this);
      IndexCountMax = idx;
    } else {
      ExpandVertices(idx, (PD::ptr)this);
      IndexCountMax += idx;
    }
  }

  void Reset() {
    Layer = 0;
    Tex = 0;
    FirstIndex = 0;
    FirstVertex = 0;
    IndexCount = 0;
    VertexCount = 0;
    VertexCountMax = 0;
    IndexCountMax = 0;
  }

  Command& Add(const Vertex& vtx) {
    if (VertexCount <= VertexCountMax)
      PutVertex(FirstVertex + VertexCount++, vtx, (PD::ptr)this);
    return *this;
  }
  Command& Add(u16 idx) {
    if (IndexCount <= IndexCountMax)
      PutIndex(FirstIndex + IndexCount++, VertexCount + idx, (PD::ptr)this);
    return *this;
  }
  Command& Add(u16 a, u16 b, u16 c) {
    if (IndexCount + 3 <= IndexCountMax) {
      size_t idx = FirstIndex + IndexCount;
      PutIndex(idx + 0, VertexCount + a, (PD::ptr)this);
      PutIndex(idx + 1, VertexCount + b, (PD::ptr)this);
      PutIndex(idx + 2, VertexCount + c, (PD::ptr)this);
      IndexCount += 3;
    }
    return *this;
  }

  int Layer = 0;
  ptr Tex = 0;
  size_t FirstVertex = 0;
  size_t FirstIndex = 0;
  size_t VertexCount = 0;
  size_t IndexCount = 0;
  // Todo: implement
  size_t VertexCountMax = 0;
  size_t IndexCountMax = 0;
};
}  // namespace Li
}  // namespace PD