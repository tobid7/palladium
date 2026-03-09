#pragma once

#include <pd/core/fnv.hpp>
#include <pd/core/strings.hpp>

namespace PD {
template <typename T>
class HashID {
 public:
  using __Type = T;

  constexpr HashID() {};
  constexpr HashID(T id) { pID = id; }
  HashID(const std::string& name) {
    pID = Detail::FNV1A<T>::Hash(name);
#ifdef PD_HASHID_KEEP_STR
    pName = name;
#endif
  }
  constexpr HashID(const char* name) {
    pID = Detail::FNV1A<T>::Hash(std::string_view(name));
#ifdef PD_HASHID_KEEP_STR
    pName = name;
#endif
  }
  ~HashID() {}

  constexpr T Get() { return pID; }

  std::string GetName() const {
#ifdef PD_HASHID_KEEP_STR
    return pName;
#else
    return std::format("hash({:#08x})", pID);
#endif
  }

  operator T() const { return pID; }
  // operator std::string() const { return GetName(); }

 private:
  T pID;
#ifdef PD_HASHID_KEEP_STR
  str pName;
#endif
};

using HashID32 = HashID<u32>;
using HashID64 = HashID<u64>;
}  // namespace PD