#pragma once

#include <pd/common/common.hpp>
#include <pd/common/strings.hpp>

namespace PD {
namespace UI7 {
class ID {
 public:
  ID(const std::string& text) {
    id = PD::Strings::FastHash(text);
    name = text;
  }
  ID(const char* text) {
    id = PD::Strings::FastHash(text);
    name = text;
  }
  ~ID() {}

  std::string GetName() const { return name; }

  operator u32() const { return id; }

 private:
  u32 id;
  std::string name;
};
}  // namespace UI7
}  // namespace PD