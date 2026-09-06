#pragma once

#include <cstddef>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
PD_API Vertex* AllocateVertices(size_t count);
PD_API u16* AllocateIndices(size_t count);
PD_API void ResetPools();
}  // namespace Li
}  // namespace PD