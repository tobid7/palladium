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

  /**
   * Set a fallback font if you dont want to explicitly set a font for every
   * objectt individually
   */
  void SetFont(PD::Li::Font& font) { pFont = &font; }
  void Render(PD::Li::Drawlist& list);

 private:
  PD::Li::Font* pFont = nullptr;
};
}  // namespace Ultra
}  // namespace PD