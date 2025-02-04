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

#include <3ds.h>

#include <pd/common/common.hpp>
#include <pd/common/error.hpp>

namespace PD {
template <typename T>
class LinearAllocator : public std::allocator<T> {
 public:
  using size_type = size_t;
  using pointer = T*;
  using const_pointer = const T*;

  template <typename T1>
  struct rebind {
    using other = LinearAllocator<T1>;
  };

  pointer allocate(size_type n, const void* hint = nullptr) {
    if (n > this->max_size()) {
      Error("Linear Alloc failed (no space left)");
      return nullptr;
    }
    return (pointer)linearAlloc(n * sizeof(T));
  }
  void deallocate(pointer p, size_type) { linearFree((void*)p); }
  size_type max_size() { return linearSpaceFree(); }

  LinearAllocator() noexcept {}
  LinearAllocator(const LinearAllocator<T>& a) noexcept
      : std::allocator<T>(a) {}
  ~LinearAllocator() noexcept {}
};
}  // namespace PD