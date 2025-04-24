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

namespace PD {
/**
 * Custom Allocator for Custom Vec and probably other stuff in future
 */
template <typename T>
class Allocator {
 public:
  Allocator() = default;
  ~Allocator() = default;

  virtual T* Allocate(size_t n) { return new T[n]; }
  virtual T* AllocateRaw(size_t n) {
    return reinterpret_cast<T*>(::operator new(sizeof(T) * n));
  }
  virtual void DeallocateRaw(T* ptr) { operator delete(ptr); }
  virtual void Deallocate(T* ptr) { delete[] ptr; }
  template <typename... Args>
  void Construct(T* ptr, Args&&... args) {
    new (ptr) T(std::forward<Args>(args)...);
  }
  void Destroy(T* ptr) { ptr->~T(); }
};
}  // namespace PD