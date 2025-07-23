#include <lazyvec.hpp>

constexpr std::string_view _funcs = R"text(
  double Len() const {{ return std::sqrt(SqLen()); }}
  double SqLen() const {{ return {1}; }}

  template <typename T1>
  double Distance(const vec{0}<T1>& v) const {{
    return (*this - v).Len();
  }}
    
  vec{0}<T> Normalize() const {{
    double l = Len();
    if(l == 0) {{
      return *this;
    }}
    return *this / (T)l;
  }}

  template <typename T1>
  T Dot(const vec{0}<T1>&v) const {{
    return {2};
  }}
)text";

constexpr std::string_view _cross = R"text(
  template <typename T1>
  vec3<T> Cross(const vec3<T1>& v) const {
    return vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }
)text";

namespace LVec {
std::string MakeFunctions(int n) {
  std::stringstream s1, s2;
  for (int i = 0; i < n; i++) {
    s1 << elems[i] << " * " << elems[i];
    s2 << elems[i] << " * (T)v." << elems[i];
    if (i != n - 1) {
      s1 << " + ";
      s2 << " + ";
    }
  }
  std::string ret = std::format(_funcs, n, s1.str(), s2.str());
  if (n == 3) {
    ret += _cross;
  }
  return ret;
}
}  // namespace LVec