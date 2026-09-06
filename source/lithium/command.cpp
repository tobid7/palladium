#include <pd/drivers/gfx.hpp>
#include <pd/lithium/command.hpp>

namespace PD {
namespace Li {

void Command::Reserve(size_t vtx, size_t idx) {
  if (VertexCountMax == 0) {
    FirstVertex = Gfx::AllocateVertices(vtx, (PD::ptr)this);
    VertexCountMax = vtx;
  } else {
    Gfx::ExpandVertices(vtx, (PD::ptr)this);
    VertexCountMax += vtx;
  }
  if (IndexCountMax == 0) {
    FirstIndex = Gfx::AllocateIndices(idx, (PD::ptr)this);
    IndexCountMax = idx;
  } else {
    Gfx::ExpandIndices(idx, (PD::ptr)this);
    IndexCountMax += idx;
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
  if (VertexCount <= VertexCountMax)
    Gfx::PutVertex(FirstVertex + VertexCount++, vtx, (PD::ptr)this);
  return *this;
}
Command& Command::Add(u16 idx) {
  if (IndexCount <= IndexCountMax)
    Gfx::PutIndex(FirstIndex + IndexCount++, FirstVertex + VertexCount + idx,
                  (PD::ptr)this);
  return *this;
}
Command& Command::Add(u16 a, u16 b, u16 c) {
  if (IndexCount + 3 <= IndexCountMax) {
    size_t idx = FirstIndex + IndexCount;
    Gfx::PutIndex(idx + 0, FirstVertex + VertexCount + a, (PD::ptr)this);
    Gfx::PutIndex(idx + 1, FirstVertex + VertexCount + b, (PD::ptr)this);
    Gfx::PutIndex(idx + 2, FirstVertex + VertexCount + c, (PD::ptr)this);
    IndexCount += 3;
  }
  return *this;
}

}  // namespace Li
}  // namespace PD