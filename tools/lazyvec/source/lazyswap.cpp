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
#include <unordered_set>

namespace LVec {
std::string MakeSwap(int n) {
  std::stringstream s;
  std::unordered_set<std::string> done;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      std::string a = elems[i];
      std::string b = elems[j];
      /** Make sure we generate nothing twice */
      if (a == b || done.count(b + a)) {
        continue;
      }
      s << "  void Swap" << (char)toupper(a[0]) << (char)toupper(b[0])
        << "() {\n";
      s << "    T t = " << a << ";\n    " << a << " = " << b << ";\n";
      s << "    " << b << " = t;\n  }\n";
      done.insert(a + b);
    }
  }
  return s.str();
}
}  // namespace LVec