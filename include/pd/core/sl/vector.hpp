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
#include <pd/core/sl/allocator.hpp>

namespace PD {
/**
 * Open Access Vector class (Alternative to std::vector)
 */
template <typename T, typename Alloc = Allocator<T>>
class Vec {
 public:
  Vec() {
    pData = pAllocator.Allocate(2);
    pCap = 2;
    Clear();
    pPos = 0;
  };
  Vec(const size_t& Size) {
    pData = pAllocator.Allocate(Size + 2);
    pCap = Size + 2;
    Clear();
    pPos = Size;
  }
  Vec(const size_t& Size, const T& v) {
    pData = pAllocator.Allocate(Size + 2);
    pCap = Size + 2;
    Clear();
    pPos = Size;
    std::fill_n(pData, Size, v);
  }
  Vec(const T* s, const T* e) {
    pData = pAllocator.Allocate(2);
    pCap = 2;
    Clear();
    pPos = 0;
    Resize(e - s);
    std::copy_n(s, e - s, pData);
  }
  Vec(const Vec& v) {
    pCap = v.pCap;
    pPos = v.pPos;
    pData = pAllocator.Allocate(pCap);
    for (size_t i = 0; i < pPos; i++) {
      pData[i] = v.pData[i];
    }
  }
  ~Vec() { pAllocator.Deallocate(pData); }

  void Add(const T& v) {
    if (pPos >= pCap) {
      Reserve(pCap * 2);
    }
    pData[pPos++] = v;
  }

  void PopBack() {
    if (pPos == 0) return;
    pPos--;
  }

  T Pop() {
    if (pPos == 0) {
      // Todo: LOG
      exit(1);
    }
    return pData[pPos--];
  }

  T& At(const size_t& Idx) {
    if (Idx >= pPos) {
      // Log
      exit(1);
    }
    return pData[Idx];
  }

  const T& At(const size_t& Idx) const {
    if (Idx >= pPos) {
      // Log
      exit(1);
    }
    return pData[Idx];
  }

  T& operator[](const size_t& Idx) { return At(Idx); }
  const T& operator[](const size_t& Idx) const { return At(Idx); }
  void operator+=(T v) { Add(v); }

  T* Begin() { return pData; }
  const T* Begin() const { return pData; }
  T* End() { return pData + pPos; }
  const T* End() const { return pData + pPos; }

  // Support: `for(auto& it : Vec)` //
  T* begin() { return pData; }
  const T* begin() const { return pData; }
  T* end() { return pData + pPos; }
  const T* end() const { return pData + pPos; }

  T* Data() { return pData; }
  T* Data() const { return pData; }
  size_t Size() const { return pPos; }
  size_t Capacity() const { return pCap; }
  void Clear() {
    // Avoid memset to support std::string for now
    // probably revert this decision based if it lacks performance
    // or make it a setting
    std::fill(pData, pData + pCap, T());
    pPos = 0;
  }

  void Reserve(const size_t& Size) {
    if (Size <= pCap) return;
    T* tmp = pAllocator.Allocate(Size);
    std::fill(tmp, tmp + Size, T());
    std::copy(pData, pData + pCap, tmp);
    pAllocator.Deallocate(pData);
    pData = tmp;
    pCap = Size;
  }

  void Resize(size_t Size) {
    if (Size < pPos) {
      pPos = Size;
    } else if (Size > pCap) {
      Reserve(Size);
    }
    std::fill(pData + pPos, pData + Size, T());
    pPos = Size;
  }

  // Allocator
  Alloc pAllocator;
  // Data Reference Pointer
  T* pData;
  // Capacity
  size_t pCap;
  // Current Position (Size)
  size_t pPos;
};
}  // namespace PD