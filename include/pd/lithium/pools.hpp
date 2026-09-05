#pragma once

#include <pd/core/core.hpp>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
PD_API PD::Pool<Vertex>& GetVertexPool();
PD_API PD::Pool<u16>& GetIndexPool();
PD_API void ResetPools();
}  // namespace Li
}  // namespace PD