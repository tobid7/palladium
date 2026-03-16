#pragma once

#include <pd/common.hpp>

namespace PD {
class DriverInterface {
 public:
  DriverInterface(const std::string_view& name) : pName(name) {}
  virtual ~DriverInterface() {}

  const char* GetName() const { return pName.data(); }

 private:
  std::string_view pName;
};
}  // namespace PD