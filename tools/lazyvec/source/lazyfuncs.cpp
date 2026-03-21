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

#include <lazyvec.hpp>

constexpr std::string_view _funcs = R"text(
  double Len() const {{ return std::sqrt(SqLen()); }}
  constexpr double SqLen() const {{ return {1}; }}

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
  constexpr T Dot(const vec{0}<T1>&v) const {{
    return {2};
  }}
)text";

constexpr std::string_view _cross = R"text(
  template <typename T1>
  constexpr vec3<T> Cross(const vec3<T1>& v) const {
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