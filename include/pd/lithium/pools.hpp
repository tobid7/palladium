#pragma once

#include <cstddef>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
/**
 * Allocate an amount of vertices
 *  returns the index of the first vertex
 */
PD_API size_t AllocateVertices(size_t count, PD::ptr accessor);
/**
 * Allocate an amount of indices
 *  returns the index of the first elem
 */
PD_API size_t AllocateIndices(size_t count, PD::ptr accessor);
PD_API bool ExpandVertices(size_t count, PD::ptr accessor);
PD_API bool ExpandIndices(size_t count, PD::ptr accessor);
PD_API void PutVertex(size_t loc, const Vertex& vtx, PD::ptr accessor);
PD_API void PutIndex(size_t loc, u16 idx, PD::ptr accessor);
PD_API const Vertex& GetVertex(size_t loc);
PD_API const u16& GetIndex(size_t loc);
PD_API void ResetPools();
}  // namespace Li
}  // namespace PD