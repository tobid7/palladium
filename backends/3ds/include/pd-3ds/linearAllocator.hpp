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

#include <3ds.h>

#include <pd/core/common.hpp>

// Custom C++ Allocator class to interface with libctru linear heap memory
// based on this guide:
// https://johnfarrier.com/custom-allocators-in-c-high-performance-memory-management/

namespace PD {
template <typename T>
class LinearAllocator {
 public:
  using value_type = T;
  LinearAllocator() noexcept = default;
  template <typename U>
  constexpr LinearAllocator(const LinearAllocator<U>&) noexcept {}

  T* allocate(std::size_t n) {
    if (n > max_size()) {
      PD::Throw("[PD] LinearAllocator: Bad alloc!");
    }
    return static_cast<T*>(linearAlloc(n * sizeof(T)));
  }
  void deallocate(T* p, std::size_t) noexcept { linearFree(p); }

  template <class U, class... Args>
  void construct(U* p, Args&&... args) {
    ::new ((void*)p) U(std::forward<Args>(args)...);
  }

  template <class U>
  void destroy(U* p) {
    p->~U();
  }

  friend bool operator==(const LinearAllocator, const LinearAllocator) {
    return true;
  }
  friend bool operator!=(const LinearAllocator, const LinearAllocator) {
    return false;
  }

  // Use linearSpace free as max_size to not allocate out of bounds
  // or to b eable to see a crash report screen.
  size_t max_size() const noexcept { return linearSpaceFree(); }
};
}  // namespace PD