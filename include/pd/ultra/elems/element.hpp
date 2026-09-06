#pragma once

#include <pd/lithium/lithium.hpp>

namespace PD {
namespace Ultra {
class ElementBase {
 public:
  ElementBase() {}
  ~ElementBase() {}

  virtual void Draw(PD::Li::Drawlist& dl) = 0;
  /**
   * Reset Function (for PD::Pool::FastReset)
   */
  virtual void Reset() {}

 private:
};
}  // namespace Ultra
}  // namespace PD