#include <pd/core/core.hpp>
#include <pd/lithium/pools.hpp>

namespace PD {
namespace Li {
PD::Pool<Vertex> pVtxPool;
PD::Pool<u16> pIdxPool;

PD_API Vertex* AllocateVertices(size_t count) {
  return pVtxPool.Allocate(count);
}

PD_API u16* AllocateIndices(size_t count) { return pIdxPool.Allocate(count); }

PD_API void ResetPools() {
  pVtxPool.Reset();
  pIdxPool.Reset();
}
}  // namespace Li
}  // namespace PD