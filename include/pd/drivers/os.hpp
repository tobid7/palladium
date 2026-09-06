#pragma once

#include <pd/drivers/interface.hpp>

namespace PD {
class PD_API OsDriver : public DriverInterface {
 public:
  OsDriver(std::string_view name = "Default") : DriverInterface(name) {}
  virtual ~OsDriver() {}

  virtual u64 GetTime() const;
  virtual u64 GetTimeNano() const;
};

class PD_API Os {
 public:
  Os() = default;
  ~Os() = default;

  template <typename T, typename... Args>
  static void UseDriver(Args&&... args) {
    // assert(driver == nullptr && "OS Driver already set");
    driver = std::make_unique<T>(std::forward<Args>(args)...);
  }

  static u64 GetTime() { return driver->GetTime(); }
  static u64 GetTimeNano() { return driver->GetTimeNano(); }

 private:
  static std::unique_ptr<OsDriver> driver;
};
}  // namespace PD