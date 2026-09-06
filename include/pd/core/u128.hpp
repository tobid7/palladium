#pragma once

#include <pd/common.hpp>

namespace PD {
/**
 * 128 Bit support for all platforms probably
 * only used for flag checks in Keyboard/Mouse Input driver
 */
class u128 {
 public:
  u64 pLow = 0;
  u64 pHigh = 0;

  constexpr u128() : pLow(0), pHigh(0) {}
  constexpr u128(u64 l, u64 h = 0) : pLow(l), pHigh(h) {}

  /**
   * Best way so far to create flags that go over 63
   * like `1 << 65` is just `u128::Flag(65)`
   */
  constexpr static u128 Flag(u32 i) {
    if (i < 64) {
      return u128(1ULL << i, 0);
    } else if (i < 128) {
      return u128(0, 1ULL << (i - 64));
    }
    return u128();
  }

  constexpr u128 operator+(const u128& v) const {
    u128 ret;
    ret.pLow = pLow + v.pLow;
    ret.pHigh = pHigh + v.pHigh + (ret.pLow < pLow);
    return ret;
  }

  constexpr u128 operator&(const u128& v) const {
    return u128(pLow & v.pLow, pHigh & v.pHigh);
  }

  constexpr u128 operator<<(u32 s) const {
    if (s == 0) {
      return *this;
    }
    if (s >= 128) {
      return u128();
    }
    if (s >= 64) {
      return u128(0, pLow << (s - 64));
    }
    return u128(pLow << s, (pHigh << s) | (pLow >> (64 - s)));
  }

  constexpr u128 operator>>(u32 s) const {
    if (s == 0) {
      return *this;
    }
    if (s >= 128) {
      return u128();
    }
    if (s >= 64) {
      return u128(pHigh >> (s - 64), 0);
    }
    return u128((pLow >> s) | (pHigh << (64 - s)), pHigh >> s);
  }

  constexpr u128& operator|=(const u128& v) {
    pLow |= v.pLow;
    pHigh |= v.pHigh;
    return *this;
  }

  constexpr u128 operator|(const u128& v) const {
    return u128(pLow | v.pLow, pHigh | v.pHigh);
  }

  constexpr u128& operator&=(const u128& v) {
    pLow &= v.pLow;
    pHigh &= v.pHigh;
    return *this;
  }

  constexpr u128 operator~() const { return u128(~pLow, ~pHigh); }

  /**
   * Old why to make if checks possible
   * Problem was that a operator& is required
   * with u128 as result
   */
  // bool operator&(const u128& v) const {
  //   return pLow & v.pLow || pHigh & v.pHigh;
  // }

  constexpr bool operator==(const u128& v) const {
    return pLow == v.pLow && pHigh == v.pHigh;
  }

  /**
   * Use explicit here to make sure it is only for checking and not for
   * some error leading implicit bool assignments...
   */
  constexpr explicit operator bool() const { return pLow != 0 || pHigh != 0; }

  /** Deprecated way to handle `flag & SomeFlag` */
  constexpr bool Has(const u128& v) const {
    return pLow & v.pLow || pHigh & v.pHigh;
  }

  constexpr bool operator!=(const u128& v) const { return !(*this == v); }
};
}  // namespace PD

namespace std {
/**
 * Provide c++ STL support for unordered map to u128
 */
template <>
struct hash<PD::u128> {
  size_t operator()(const PD::u128& k) const {
    // just combine hashes of the parts usign simple xor op
    size_t h0 = std::hash<PD::u64>{}(k.pLow);
    size_t h1 = std::hash<PD::u64>{}(k.pHigh);
    return h0 ^ (h1 << 1);
  }
};
}  // namespace std