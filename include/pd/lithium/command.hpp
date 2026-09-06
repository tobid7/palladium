#pragma once

#include <pd/core/pool.hpp>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace Li {
class Command {
 public:
  Command() {}
  ~Command() {}

  int Layer = 0;
  ptr Tex = 0;
  Vertex* FirstVertex = nullptr;
  u16* FirstIndex = nullptr;
  size_t VertexCount;
  size_t IndexCount;
  
  PD::Pool<Vertex>* pVpool;
  PD::Pool<u16>* pIpool;
};
}  // namespace Li
}  // namespace PD