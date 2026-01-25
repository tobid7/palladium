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

namespace PD {
/**
 * Class to calculate Maximum/Minimum and Average Timings
 */
class TimeStats {
 public:
  /**
   * Constructor taking a lengh for the List
   * @param l Lengh of the data list
   */
  TimeStats(int l) : len(l), val(l, 0) {}
  ~TimeStats() = default;

  PD_SHARED(TimeStats);

  /**
   * Add a New Value to the list
   * @param v value to add
   */
  void Add(u64 v) {
    val[idx] = v;
    idx = next(idx);
    num_val = std::min(num_val + 1, len);
  }

  /**
   * Get Avarage Num
   * @return Average
   */
  u64 GetAverage() {
    if (!num_val) return 0.f;
    u64 res = 0;
    for (int i = 0; i < num_val; i++) {
      res += val[smart_idx(i)];
    }
    return res / num_val;
  }

  /**
   * Get Minimum Num
   * @return Minimum value
   */
  u64 GetMin() {
    if (!num_val) return 0.f;
    u64 res = std::numeric_limits<u64>::max();
    for (int i = 0; i < num_val; i++) {
      res = std::min(val[smart_idx(i)], res);
    }
    return res;
  }

  /**
   * Get Maximum Value
   * @return Max Value
   */
  u64 GetMax() {
    if (!num_val) return 0.f;
    u64 res = 0;
    for (int i = 0; i < num_val; i++) {
      res = std::max(val[smart_idx(i)], res);
    }
    return res;
  }

  /**
   * Clear the List
   */
  void Clear() {
    val.assign(len, 0);
    idx = 0;
    num_val = 0;
  }

  /**
   * Get Data Buffer
   * @return data bufer (not edidable)
   */
  const std::vector<u64>& GetData() { return val; }
  /**
   * Access an element in the list [not edidable]
   * @return value to access
   */
  const u64& operator[](int i) { return val[smart_idx(i)]; }
  /**
   * Get List Lengh
   * @return Lengh
   */
  const size_t GetLen() { return len; }
  /**
   * Get Number of Values
   * @return number of values
   */
  const size_t GetNumValues() { return num_val; }

 private:
  /**
   * Get the Next Position to write to
   * @param c current position
   * @return next position
   */
  size_t next(size_t c) const { return (c + 1) % len; }
  /**
   * Smart Indexing in for loops to make sure to
   * not index a value that was not set yet
   * @param v pos in for loop
   * @return indexing pos
   */
  size_t smart_idx(size_t v) const { return (idx + len - num_val + v) % len; }

  /** Lengh of the list */
  int len = 0;
  /** Value Storage */
  std::vector<u64> val;
  int idx = 0;
  int num_val = 0;
};
/**
 * Timatrace Functions
 */
namespace TT {
/**
 * Data Structure for a TimeTrace Result
 */
class Res {
 public:
  /** Constructore that Inits a protocol at size of 60 frames */
  Res() : start(0), end(0) { protocol = TimeStats::New(60); }
  ~Res() = default;

  PD_SHARED(Res);

  /**
   * Setter for the ID (Name)
   * @param v ID of the Trace
   */
  void SetID(const std::string& v) { id = v; }
  /**
   * Getter for the traces ID
   * @return Trace ID
   */
  const std::string GetID() { return id; }
  /**
   * Setter for the Start Value
   * @param v start time
   */
  void SetStart(u64 v) { start = v; }
  /**
   * Getter for the Start time
   * @return start time
   */
  u64 GetStart() { return start; }
  /**
   * Setter for the End Time
   * @param v end time
   */
  void SetEnd(u64 v) {
    end = v;
    diff = end - start;
    protocol->Add(GetLastDiff());
  }
  /**
   * Getter for the End Time
   * @result end time
   */
  u64 GetEnd() { return end; }

  /**
   * Get Last Diffrence between end and start
   * @return end - start
   */
  u64 GetLastDiff() { return diff; }
  /**
   * Get Protcol Reference
   * @return Protocol Ref
   */
  TimeStats::Ref GetProtocol() { return protocol; }

 private:
  /** Trace ID */
  std::string id;
  /** Start time */
  u64 start;
  /** End Time */
  u64 end;
  /** Last Diff */
  u64 diff;
  /** Protocol */
  TimeStats::Ref protocol;
};
/**
 * Begin a Trace
 * @param id Name of the Trace
 */
PD_API void Beg(const std::string& id);
/**
 * End a Trace
 * @param id Name of the Trace
 */
PD_API void End(const std::string& id);
/**
 * Collect Start end end of the trace by tracking
 * when the Scope object goes out of scope
 *
 * Example:
 * ```cpp
 * void SomeFunction() {
 *    // Create a Scoped Trace called "SomeFunc"
 *    PD::TT::Scope st("SomeFunc");
 *    // Do your functions stuff
 *    // End at the end it goes out of
 *    // scope which collects the end time
 * }
 * ```
 */
class Scope {
 public:
  /**
   * Constructor requiring a Name for the Trace
   * @param id Name of the Trace
   */
  Scope(const std::string& id) {
    this->ID = id;
    Beg(id);
  }
  /**
   * Deconstructor getting the end time when going out of scope
   */
  ~Scope() { End(ID); }

 private:
  /** Trace Name/ID */
  std::string ID;
};
}  // namespace TT
}  // namespace PD