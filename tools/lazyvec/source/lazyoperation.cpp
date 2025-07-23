#include <lazyvec.hpp>

constexpr std::string_view _op_template = R"text(
  template <typename T1>
  vec{0}<T>& operator{1}=(T1 v) {{
{2}
    return *this;
  }}

  template <typename T1>
  vec{0}<T>& operator{1}=(const vec{0}<T1>& v) {{
{3}
    return *this;
  }}

  template <typename T1>
  vec{0}<T> operator{1}(T1 v) const {{
    return vec{0}<T>({4});
  }}

  template <typename T1>
  vec{0}<T> operator{1}(const vec{0}<T1>& v) const {{
    return vec{0}<T>({5});
  }}
)text";

namespace LVec {
std::string MakeOperationFor(char op, int n) {
  const std::string& toff = "    ";
  // Create for streams for the operations functions
  std::stringstream s1, s2, s3, s4;
  for (int i = 0; i < n; i++) {
    s1 << toff << elems[i] << " " << op << "= (T)v;";
    s2 << toff << elems[i] << " " << op << "= (T)v." << elems[i] << ";";
    s3 << elems[i] << " " << op << " (T)v";
    s4 << elems[i] << " " << op << " (T)v." << elems[i];
    if (i != n - 1) {
      s1 << std::endl;
      s2 << std::endl;
      s3 << ", ";
      s4 << ", ";
    }
  }
  return std::format(_op_template, n, op, s1.str(), s2.str(), s3.str(),
                     s4.str());
}
}  // namespace LVec