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

#include <pd/common/common.hpp>

namespace PD {
class TimeStats : public SmartCtor<TimeStats> {
 public:
  TimeStats(int l) : len(l), val(l, 0) {}
  ~TimeStats() = default;

  void Add(u64 v) {
    val[idx] = v;
    idx = next(idx);
    num_val = std::min(num_val + 1, len);
  }

  u64 GetAverage() {
    if (!num_val) return 0.f;
    u64 res = 0;
    for (int i = 0; i < num_val; i++) {
      res += val[smart_idx(i)];
    }
    return res / num_val;
  }

  u64 GetMin() {
    if (!num_val) return 0.f;
    u64 res = std::numeric_limits<u64>::max();
    for (int i = 0; i < num_val; i++) {
      res = std::min(val[smart_idx(i)], res);
    }
    return res;
  }

  u64 GetMax() {
    if (!num_val) return 0.f;
    u64 res = 0;
    for (int i = 0; i < num_val; i++) {
      res = std::max(val[smart_idx(i)], res);
    }
    return res;
  }

  void Clear() {
    val.assign(len, 0);
    idx = 0;
    num_val = 0;
  }

  const std::vector<u64> &GetData() { return val; }
  const u64 &operator[](int i) { return val[smart_idx(i)]; }
  const size_t GetLen() { return len; }
  const size_t GetNumValues() { return num_val; }

 private:
  size_t next(size_t c) const { return (c + 1) % len; }
  size_t smart_idx(size_t v) const { return (idx + len - num_val + v) % len; }

  int len = 0;
  std::vector<u64> val;
  int idx = 0;
  int num_val = 0;
};
namespace TT {
class Res : public SmartCtor<Res> {
 public:
  Res() { protocol = TimeStats::New(60); }
  ~Res() = default;

  void SetID(const std::string &v) { id = v; }
  const std::string GetID() { return id; }
  void SetStart(u64 v) { start = v; }
  u64 GetStart() { return start; }
  void SetEnd(u64 v) {
    end = v;
    protocol->Add(GetLastDiff());
  }
  u64 GetEnd() { return end; }

  u64 GetLastDiff() { return end - start; }
  TimeStats::Ref GetProtocol() { return protocol; }

 private:
  std::string id;
  u64 start;
  u64 end;
  TimeStats::Ref protocol;
};
void Beg(const std::string &id);
void End(const std::string &id);
class Scope {
 public:
  Scope(const std::string &id) {
    this->id = id;
    Beg(id);
  }
  ~Scope() { End(id); }

 private:
  std::string id;
};
}  // namespace TT
}  // namespace PD