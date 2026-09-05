#include <pd/lithium/pools.hpp>

namespace PD {
namespace Li {
static PD::Pool<Vertex> gVertexPool;
static PD::Pool<u16> gIndexPool;

PD_API PD::Pool<Vertex>& GetVertexPool() { return gVertexPool; }
PD_API PD::Pool<u16>& GetIndexPool() { return gIndexPool; }

PD_API void ResetPools() {
  gVertexPool.NoReset();
  gIndexPool.NoReset();
}
}  // namespace Li
}  // namespace PD