#pragma once

#include <pd/ultra/canvas.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
class PD_API Layout {
 public:
  Layout() {}
  ~Layout() {}

  void Add(ElementBase& elem);

  void SetFont(PD::Li::Font& font);

  void Render();

  const PD::Li::Drawlist& Data() const { return pList; }
  Canvas& GetCanvas() { return pCanvas; }
  const Canvas& GetCanvas() const { return pCanvas; }

 private:
  Canvas pCanvas;
  PD::Pool<ElementBase*> pElements;
  PD::Li::Drawlist pList;
};
}  // namespace Ultra
}  // namespace PD