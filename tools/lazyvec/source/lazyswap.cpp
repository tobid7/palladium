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