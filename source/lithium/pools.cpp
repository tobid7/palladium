#include <pd/lithium/pools.hpp>

#include "pd/common.hpp"
#include "pd/core/pool.hpp"
#include "pd/lithium/vertex.hpp"

namespace PD {
namespace Li {
PD::Pool<Vertex> pVtxPool;
PD::Pool<u16> pIdxPool;

void InitPools(size_t max_vertices) {
  pVtxPool.Init(max_vertices);
  pIdxPool.Init(max_vertices * 2);
}

Vertex* AllocateVertices(size_t count) { return pVtxPool.Allocate(count); }

u16* AllocateIndices(size_t count) { return pIdxPool.Allocate(count); }

void ResetPools() {
  pVtxPool.Reset();
  pIdxPool.Reset();
}
}  // namespace Li
}  // namespace PD