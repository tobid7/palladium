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

  void SetFont(PD::Li::Font& font);

  void Render();

  const PD::Li::Drawlist& Data() const { return pList; }

 private:
  PD::Li::Drawlist pList;
};
}  // namespace Ultra
}  // namespace PD