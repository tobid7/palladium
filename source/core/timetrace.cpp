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

#include <pd/core/timetrace.hpp>
#include <pd/drivers/os.hpp>

namespace PD::TT {
static TraceMap pTraces;

PD_API TraceMap& GetTraceMap() { return pTraces; }

PD_API TT::Res& GetTraceRef(const std::string& id) {
  if (!pTraces.count(id)) {
    pTraces[id] = TT::Res();
    pTraces[id].SetID(id);
  }
  return pTraces[id];
}

PD_API bool TraceExist(const std::string& id) { return pTraces.count(id); }

PD_API void Beg(const std::string& id) {
  auto& trace = GetTraceRef(id);
  trace.SetStart(PD::Os::GetTimeNano());
}

PD_API void End(const std::string& id) {
  auto& trace = GetTraceRef(id);
  trace.SetEnd(PD::Os::GetTimeNano());
}
}  // namespace PD::TT