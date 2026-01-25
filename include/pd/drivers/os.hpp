#pragma once

/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <pd/core/common.hpp>
#include <pd/core/timetrace.hpp>

namespace PD {
using TraceMap = std::map<std::string, TT::Res::Ref>;

class OsDriver {
 public:
  OsDriver() = default;
  virtual ~OsDriver() = default;
  PD_SHARED(OsDriver);

  virtual u64 GetTime();
  virtual u64 GetNanoTime();
  TraceMap& GetTraceMap();
  TT::Res::Ref& GetTraceRef(const std::string& id);
  bool TraceExist(const std::string& id);

  TraceMap pTraces;
};

/** Static Os Controller */
class OS {
 public:
  OS() = default;
  ~OS() = default;

  static void Init(OsDriver::Ref v = nullptr) {
    if (v) {
      pOs = v;
    } else {
      pOs = OsDriver::New();
    }
  }

  static u64 GetTime() { return pOs->GetTime(); }
  static u64 GetNanoTime() { return pOs->GetNanoTime(); }
  static TraceMap& GetTraceMap() { return pOs->GetTraceMap(); }
  static TT::Res::Ref& GetTraceRef(const std::string& id) {
    return pOs->GetTraceRef(id);
  }
  static bool TraceExist(const std::string& id) { return pOs->TraceExist(id); }

  static OsDriver::Ref pOs;
};
}  // namespace PD