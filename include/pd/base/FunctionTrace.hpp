#pragma once
// Base includes
#include <functional>
#include <map>
#include <string>

// 3ds does not support std::chrono
#include <3ds.h>

/// @brief 3ds System Ticks per milli second
#define TICKS_PER_MSEC 268111.856

#define f2s(x_) #x_
#define scomb(x1, x2) std::string(x1 + x2)

namespace Palladium {
namespace Ftrace {
class TimeStats {
 public:
  TimeStats(int len) : len(len), values(len, 0) {}

  void Add(float v) {
    values[idx] = v;
    idx = next_index(idx);
    num_values = std::min(num_values + 1, len);
  }

  float GetAverage() {
    float res = 0.f;
    if (!num_values) {
      return res;
    }
    for (int i = 0; i < num_values; ++i) {
      res += values[index(i)];
    }
    return res / num_values;
  }

  float GetMax() {
    float res = 0.f;
    if (!num_values) {
      return res;
    }
    for (int i = 0; i < num_values; i++) {
      res = std::max(res, values[index(i)]);
    }
    return res;
  }

  float GetMin() {
    float res = 0.f;
    if (!num_values) {
      return res;
    }
    res = values[0];
    for (int i = 0; i < num_values; i++) {
      res = std::min(res, values[index(i)]);
    }
    return res;
  }

  const std::vector<float>& GetData() { return values; }
  const float& operator[](int i) { return values[index(i)]; }
  const size_t GetLen() { return len; }
  const size_t GetNumValues() { return num_values; }

 private:
  // Indexing Functions for better overview
  size_t next_index(size_t current) const { return (current + 1) % len; }
  size_t index(size_t v) const { return (idx + len - num_values + v) % len; }

  // Data
  int len = 0;
  std::vector<float> values;
  int idx = 0;
  int num_values = 0;
};
/// @brief Result of FTrace
struct FTRes {
  FTRes() : time_start(0), time_end(0), time_of(0.f), is_ovl(false), ts(60) {}
  std::string group;      ///< Group of the Trace
  std::string func_name;  ///< Function Name

  uint64_t time_start;  ///< when started
  uint64_t time_end;    ///< when stopped
  float time_of;        ///< stop - start (how long)
  bool is_ovl;          ///< is displayed in overlay?
  TimeStats ts;         ///< Time Stats
};

/// @brief Map of Traces
extern std::map<std::string, Palladium::Ftrace::FTRes> pd_traces;

/// @brief Set a Start TracePoint
/// @param group Set a Group Name
/// @param func_name Set a Function Name
inline void Beg(const std::string& group, const std::string& func_name) {
  std::string trace_id = scomb(group, func_name);
  auto& trace = pd_traces[trace_id];
  trace.group = group;
  trace.func_name = func_name;
  trace.time_start = svcGetSystemTick();
}
/// @brief Set an End TracePoint
/// @param group Set a Group Name
/// @param func_name Set a Function Name
inline void End(const std::string& group, const std::string& func_name) {
  std::string trace_id = scomb(group, func_name);
  auto& trace = pd_traces[trace_id];
  trace.time_end = svcGetSystemTick();
  trace.time_of =
      static_cast<float>(trace.time_end - trace.time_start) / TICKS_PER_MSEC;
  trace.ts.Add(trace.time_of);
}
/// @brief Trace a function execution
/// @param group Set a Group Name
/// @param name Set a Function Name
inline void Func(const std::string& group, const std::string& name,
                 std::function<void()> fun) {
  if (!fun) return;
  Beg(group, name);
  fun();
  End(group, name);
}

/// @brief This Starts an Ftrace and
/// end ist when going out of scope
struct ScopedTrace {
  ScopedTrace(std::string g, std::string n) : group(g), name(n) {
    Ftrace::Beg(g, n);
  }
  ~ScopedTrace() { Ftrace::End(group, name); }
  std::string group;
  std::string name;
};
}  // namespace Ftrace
}  // namespace Palladium
