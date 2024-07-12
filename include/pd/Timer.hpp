#pragma once

#include <3ds.h>

#include <pd/smart_ctor.hpp>

namespace Palladium {
class Timer {
 public:
  Timer(bool autostart = true);
  ~Timer() {}
  PD_SMART_CTOR(Timer)
  void Reset();
  void Tick();
  void Pause();
  void Resume();
  float Get();
  float GetLive();
  bool Running();

 private:
  uint64_t last = 0;
  uint64_t current = 0;
  bool is_running = false;
};
}  // namespace Palladium