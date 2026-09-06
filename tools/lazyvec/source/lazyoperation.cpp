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

constexpr std::string_view _op_template = R"text(
  template <typename T1>
  constexpr vec{0}<T>& operator{1}=(T1 v) {{
{2}
    return *this;
  }}

  template <typename T1>
  constexpr vec{0}<T>& operator{1}=(const vec{0}<T1>& v) {{
{3}
    return *this;
  }}

  template <typename T1>
  constexpr vec{0}<T> operator{1}(T1 v) const {{
    return vec{0}<T>({4});
  }}

  template <typename T1>
  constexpr vec{0}<T> operator{1}(const vec{0}<T1>& v) const {{
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