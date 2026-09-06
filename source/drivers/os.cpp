#include <pd/drivers/os.hpp>

namespace PD {
PD_API std::unique_ptr<OsDriver> Os::driver = std::make_unique<OsDriver>();

PD_API u64 OsDriver::GetTime() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

PD_API u64 OsDriver::GetTimeNano() const {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
}  // namespace PD