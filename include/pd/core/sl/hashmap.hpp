#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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
#include <pd/core/sl/list.hpp>
#include <pd/core/sl/pair.hpp>

namespace PD {
template <typename K, typename V, size_t bucket_count = 10>
class HashMap {
 public:
  HashMap() {}
  ~HashMap() {}

  void Insert(const K& k, const V& v) {
    size_t idx = Hash(k);
    auto& bukket = pBuckets[idx];
    for (auto& it : bukket) {
      if (it.First == k) {
        it.Second = v;
        return;
      }
    }
    bukket.PushBack(Pair(k, v));
  }

  bool Contains(const K& k) const {
    size_t idx = Hash(k);
    auto& bukket = pBuckets[idx];

    for (auto& it : bukket) {
      if (it.First == k) {
        return true;
      }
    }
    return false;
  }

  void Clear() {
    for (size_t i = 0; i < bucket_count; i++) {
      pBuckets[i].Clear();
    }
  }

  size_t Hash(const K& k) const { return std::hash<K>{}(k) % bucket_count; }
  PD::List<PD::Pair<K, V>> pBuckets[bucket_count];
};
}  // namespace PD