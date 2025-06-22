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
#include <pd/core/sl/vector.hpp>

namespace PD {
/**
 * Custom Stack class (caus std::stack ofsten lead to memory coruption)
 */
template <typename T, typename Alloc = Allocator<T>>
class Stack {
 public:
  Stack() = default;
  explicit Stack(size_t cap) : pVec(cap) {}

  void Push(const T& val) { pVec.Add(val); }

  void Pop() {
    if (pVec.Size() == 0) {
      exit(1);
    }
    pVec.PopBack();
  }

  T& Top() {
    if (pVec.Size() == 0) {
      exit(1);
    }
    return pVec[pVec.Size() - 1];
  }

  const T& Top() const {
    if (pVec.Size() == 0) {
      exit(1);
    }
    return pVec[pVec.Size() - 1];
  }

  bool IsEmpty() const { return pVec.Size() == 0; }
  size_t Size() const { return pVec.Size(); }
  void Clear() { pVec.Clear(); }

 private:
  Vec<T, Alloc> pVec;
};
}  // namespace PD