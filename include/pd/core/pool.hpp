#pragma once

#include <memory>
#include <pd/common.hpp>
namespace PD {
template <typename T, typename Alloc = std::allocator<T>>
class Pool {
 public:
  using value_type = T;
  using iterator = T*;
  using const_iterator = const T*;

  Pool() = default;
  ~Pool() {
    if (pData) {
      pAlloc.deallocate(pData, pCap);
      pData = nullptr;
    }
  }

  static Pool* Create(size_t size) { return new Pool(size); }

  void Init(size_t size) {
    pPos = 0;
    pCap = size;
    pData = pAlloc.allocate(size);
    for (size_t i = 0; i < pCap; i++) {
      std::allocator_traits<Alloc>::construct(pAlloc, &pData[i]);
    }
    PDLOG("Pool::Init({})", size);
  }

  Pool(const Pool&) = delete;
  Pool(Pool&&) = delete;
  Pool& operator=(const Pool&) = delete;
  Pool& operator=(Pool&&) = delete;

  T* Allocate(size_t num = 1) {
    ExpandIf(num);
    T* ret = &pData[pPos];
    pPos += num;
    return ret;
  }

  void Push(const T& elem) {
    T* e = Allocate(1);
    *e = elem;
  }

  void ExpandIf(size_t req) {
    if ((pPos + req) <= pCap) return;
    size_t ncap = std::max(pCap * 2, pPos + req);
    T* nu = pAlloc.allocate(ncap);
    if (pData) {
      for (size_t i = 0; i < pPos; i++) {
        std::allocator_traits<Alloc>::construct(
            pAlloc, &nu[i], std::move_if_noexcept(pData[i]));
      }
      pAlloc.deallocate(pData, pCap);
    }
    for (size_t i = pPos; i < ncap; i++) {
        std::allocator_traits<Alloc>::construct(pAlloc, &nu[i]);
      }
    PDLOG("Pool::ExpandIf({}): {} -> {}", req, pCap, ncap);
    pData = nu;
    pCap = ncap;
  }

  void Reset() {
    for (size_t i = 0; i < pCap; i++) {
      std::allocator_traits<Alloc>::destroy(pAlloc, &pData[i]);
    }
    pPos = 0;
    for (size_t i = 0; i < pCap; i++) {
      std::allocator_traits<Alloc>::construct(pAlloc, &pData[i]);
    }
  }

  size_t size() const { return pPos; }
  size_t capacity() const { return pCap; }
  T& at(size_t idx) { return pData[idx]; }
  const T& at(size_t idx) const { return pData[idx]; }
  iterator begin() { return pData; }
  iterator end() { return pData + pPos; }
  const_iterator begin() const { return pData; }
  const_iterator end() const { return pData + pPos; }

  T& operator[](size_t idx) { return at(idx); }
  const T& operator[](size_t idx) const { return at(idx); }

 private:
  Pool(size_t size) : pCap(size), pPos(0) { pData = pAlloc.allocate(size); }
  size_t pCap = 0;
  size_t pPos = 0;
  Alloc pAlloc;
  T* pData = nullptr;
};
}  // namespace PD