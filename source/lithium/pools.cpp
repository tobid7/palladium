#include <pd/core/core.hpp>
#include <pd/lithium/pools.hpp>

namespace PD {
namespace Li {
PD::Pool<Vertex> pVtxPool;
PD::Pool<u16> pIdxPool;
PD::ptr pVertexAccessor = 0;
PD::ptr pIndexAccessor = 0;

PD_API size_t AllocateVertices(size_t count, PD::ptr accessor) {
  pVertexAccessor = accessor;
  int loc = pVtxPool.size();
  pVtxPool.Allocate(count);
  return loc;
}

PD_API size_t AllocateIndices(size_t count, PD::ptr accessor) {
  pIndexAccessor = accessor;
  int loc = pIdxPool.size();
  pIdxPool.Allocate(count);
  return loc;
}

PD_API bool ExpandVertices(size_t count, PD::ptr accessor) {
  if (pVertexAccessor != accessor) return false;
  pVtxPool.Allocate(count);
  return true;
}

PD_API bool ExpandIndices(size_t count, PD::ptr accessor) {
  if (pIndexAccessor != accessor) return false;
  pIdxPool.Allocate(count);
  return true;
}

PD_API void PutVertex(size_t loc, const Vertex& vtx, PD::ptr accessor) {
  if (pVertexAccessor != accessor) return;
  pVtxPool.Put(loc, vtx);
}

PD_API void PutIndex(size_t loc, u16 idx, PD::ptr accessor) {
  if (pIndexAccessor != accessor) return;
  pIdxPool.Put(loc, idx);
}

PD_API const Vertex& GetVertex(size_t loc) {
  if (loc < pVtxPool.size()) return pVtxPool[loc];
  static Vertex pErrVtx;
  return pErrVtx;
}

PD_API const u16& GetIndex(size_t loc) {
  if (loc < pIdxPool.size()) return pIdxPool[loc];
  static u16 pErrIdx = 0;
  return pErrIdx;
}

PD_API void ResetPools() {
  pVtxPool.Reset();
  pIdxPool.Reset();
}
}  // namespace Li
}  // namespace PD