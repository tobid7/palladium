#pragma once

#include <pd/ultra/canvas.hpp>
#include <pd/ultra/container.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
class PD_API Layout : public Container {
 public:
  Layout() {}
  ~Layout() {}

  void Render(PD::Li::Drawlist& list);
};
}  // namespace Ultra
}  // namespace PD