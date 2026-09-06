#pragma once

#include <pd/common.hpp>

namespace PD {
template <typename T, typename Alloc = std::allocator<T>>
class Pool {
 public:
  Pool() = default;
  ~Pool() = default;

  static Pool* Create(size_t size) { return new Pool(size); }

  void Init(size_t size) {
    pPos = 0;
    pCap = size;
    pPool.resize(size);
  }

  Pool(const Pool&) = delete;
  Pool(Pool&&) = delete;
  Pool& operator=(const Pool&) = delete;
  Pool& operator=(Pool&&) = delete;

  T* Allocate(size_t num = 1) {
    if (CheckLimits(num)) return nullptr;
    T* ret = &pPool[pPos];
    pPos += num;
    return ret;
  }

  bool CheckLimits(size_t num) {
    if ((pPos + num) >= pCap) {
      throw std::runtime_error(
          std::format("[PD::Pool]: Trying to allocate {} elements but this is "
                      "going out of range ({}/{})",
                      num, pPos + num, pCap));
      return true;
    }
    return false;
  }

  void Reset() { pPos = 0; }

 private:
  Pool(size_t size) : pCap(size), pPos(0) { pPool.resize(size); }
  size_t pCap = 0;
  size_t pPos = 0;
  std::vector<T, Alloc> pPool;
};
}  // namespace PD