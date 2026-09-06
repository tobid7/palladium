#pragma once

#include <cstddef>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
PD_API void InitPools(size_t max_vertices = 32768);
PD_API Vertex* AllocateVertices(size_t count);
PD_API u16* AllocateIndices(size_t count);
}  // namespace Li
}  // namespace PD