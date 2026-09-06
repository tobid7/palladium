#pragma once

#include <cstddef>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
void InitPools(size_t max_vertices = 32768);
Vertex* AllocateVertices(size_t count);
u16* AllocateIndices(size_t count);
}  // namespace Li
}  // namespace PD