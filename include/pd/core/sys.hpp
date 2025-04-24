#pragma once

/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

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
/**
 * Namespace containing functions for get Millis and Get Nanos
 */
namespace Sys {
/**
 * alias for the TimeTrace Traces Map
 */
using TraceMap = std::map<std::string, TT::Res::Ref>;
/**
 * Get Current Time in Milliseconds
 * @return 64Bit value of millis
 */
PD_CORE_API u64 GetTime();
/**
 * Get Current Time in Nanoseconds
 * @return 64Bit value of nanos
 */
PD_CORE_API u64 GetNanoTime();
/**
 * Get a TimeTrace Reference by its string ID
 * @param id trace name
 * @return Trace reference or nullptr if not found
 */
PD_CORE_API TT::Res::Ref& GetTraceRef(const std::string& id);
/**
 * Check if a Trace with the name exists
 * @param id tracename to search
 * @return true if exist
 */
PD_CORE_API bool TraceExist(const std::string& id);
/**
 * Get TraceMap Reference
 * @return edidable Reference to the TraceMap
 */
PD_CORE_API TraceMap& GetTraceMap();
}  // namespace Sys
}  // namespace PD
