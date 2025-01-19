#pragma once

#include <pd/common/common.hpp>
#include <pd/common/strings.hpp>

namespace PD {
namespace UI7 {
class ID {
 public:
  ID(const std::string& text) { id = PD::Strings::FastHash(text); }
  ~ID() {}

  operator u32() const { return id; }

 private:
  u32 id;
};
}  // namespace UI7
}  // namespace PD