#include <pd/drivers/gfx.hpp>
#include <pd/lithium/command.hpp>
#include <pd/lithium/pools.hpp>

namespace PD {
namespace Li {

void Command::Reserve(size_t vtx, size_t idx) {
  auto& vpool = GetVertexPool();
  auto& ipool = GetIndexPool();
  if (VertexCountMax == 0) {
    FirstVertex = vpool.size();
    vpool.Allocate(vtx);
    VertexCountMax = vtx;
  } else {
    if (vpool.size() == FirstVertex + VertexCountMax) {
      vpool.Allocate(vtx);
      VertexCountMax += vtx;
    } else {
      size_t tmp = FirstVertex;
      FirstVertex = vpool.size();
      vpool.Allocate(VertexCountMax + vtx);
      for (size_t i = 0; i < VertexCount; i++) {
        vpool[FirstVertex + i] = vpool[tmp + i];
      }
      VertexCountMax += vtx;
    }
  }
  if (IndexCountMax == 0) {
    FirstIndex = ipool.size();
    ipool.Allocate(idx);
    IndexCountMax = idx;
  } else {
    if (ipool.size() == FirstIndex + IndexCountMax) {
      ipool.Allocate(idx);
      IndexCountMax += idx;
    } else {
      size_t tmp = FirstIndex;
      FirstIndex = ipool.size();
      ipool.Allocate(IndexCountMax + idx);
      for (size_t i = 0; i < IndexCount; i++) {
        ipool[FirstIndex + i] = ipool[tmp + i];
      }
      IndexCountMax += idx;
    }
  }
}

void Command::Reset() {
  Layer = 0;
  Tex = 0;
  SDF = false;
  FirstIndex = 0;
  FirstVertex = 0;
  IndexCount = 0;
  VertexCount = 0;
  VertexCountMax = 0;
  IndexCountMax = 0;
}

Command& Command::Add(const Vertex& vtx) {
  if (VertexCount < VertexCountMax) {
    GetVertexPool()[FirstVertex + VertexCount++] = vtx;
  }
  return *this;
}
Command& Command::Add(u16 idx) {
  if (IndexCount < IndexCountMax) {
    GetIndexPool()[FirstIndex + IndexCount++] =
        static_cast<u16>(VertexCount + idx);
  }
  return *this;
}
Command& Command::Add(u16 a, u16 b, u16 c) {
  if (IndexCount + 3 <= IndexCountMax) {
    auto& ip = GetIndexPool();
    size_t idx = FirstIndex + IndexCount;
    ip[idx + 0] = static_cast<u16>(VertexCount + a);
    ip[idx + 1] = static_cast<u16>(VertexCount + b);
    ip[idx + 2] = static_cast<u16>(VertexCount + c);
    IndexCount += 3;
  }
  return *this;
}

}  // namespace Li
}  // namespace PD