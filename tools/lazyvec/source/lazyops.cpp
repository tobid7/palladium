#include <lazyvec.hpp>

/**
 * Diffrence between lazyoperation.cpp and lazyops.cpp ?
 * One is for Operators with an input sym like +, -, * or /
 * the other (this) is for generic operations
 */

constexpr std::string_view _generic_ops = R"text(
  vec{0} operator-() const {{ return vec{0}({1}); }}
  template <typename T1>
  bool operator==(const vec{0}<T1>& v) const {{ return {2}; }}
  template <typename T1>
  bool operator!=(const vec{0}<T1>& v) const {{ return !(*this == v); }}
)text";

namespace LVec {
std::string GenericOperations(int n) {
  std::stringstream s1, s2;
  for (int i = 0; i < n; i++) {
    s1 << "-" << elems[i];
    s2 << elems[i] << " == (T)v." << elems[i];
    if (i != n - 1) {
      s1 << ", ";
      s2 << " && ";
    }
  }
  return std::format(_generic_ops, n, s1.str(), s2.str());
}
}  // namespace LVec