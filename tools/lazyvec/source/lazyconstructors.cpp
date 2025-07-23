#include <lazyvec.hpp>

constexpr std::string_view _construct = R"text(
  constexpr vec{0}() : {1} {{}}
  template <typename T1>
  constexpr vec{0}(T1 v) {{
    {2}
  }}

  template <typename T1>
  constexpr vec{0}(const vec{0}<T1>& v) {{
    {3}
  }}

  constexpr explicit vec{0}({4}) : {5} {{}}{6}
)text";

constexpr std::string_view _extended3 = R"(

  // Extended Constructors
  template <typename T1>
  constexpr explicit vec3(const vec2<T1>& xy, T1 z) {{
    x = (T)xy.x;
    y = (T)xy.y;
    this->z = (T)z;
  }}
)";

constexpr std::string_view _extended4 = R"(

  // Extended Constructors
  template <typename T1>
  constexpr explicit vec4(const vec2<T1>& xy, const vec2<T1>& zw) {{
    x = (T)xy.x;
    y = (T)xy.y;
    z = (T)zw.x;
    w = (T)zw.y;
  }}

  template <typename T1>
  constexpr explicit vec4(const vec3<T1>& xyz, T1 w) {{
    x = (T)xyz.x;
    y = (T)xyz.y;
    z = (T)xyz.z;
    this->w = (T)w;
  }}
)";

namespace LVec {
std::string MakeConstructors(int n) {
  std::stringstream s1, s2, s3, s4, s5;
  for (int i = 0; i < n; i++) {
    s1 << elems[i] << "(0)";
    s2 << "    " << elems[i] << " = (T)v;";
    s3 << "    " << elems[i] << " = (T)v." << elems[i] << ";";
    s4 << "T " << elems[i];
    s5 << elems[i] << "(" << elems[i] << ")";
    if (i != n - 1) {
      s1 << ", ";
      s2 << std::endl;
      s3 << std::endl;
      s4 << ", ";
      s5 << ", ";
    }
  }
  std::string extended;
  if (n == 3) {
    extended = _extended3;
  } else if (n == 4) {
    extended = _extended4;
  }
  return std::format(_construct, n, s1.str(), s2.str(), s3.str(), s4.str(),
                     s5.str(), extended);
}
}  // namespace LVec