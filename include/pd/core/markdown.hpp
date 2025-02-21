#pragma once

/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

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

#include <pd/core/common.hpp>

namespace PD {
class Markdown {
 public:
  Markdown() = default;
  ~Markdown() = default;

  void Header(const std::string& hdr, int lvl = 2) {
    if (task != 0 || lvl < 1 || lvl > 10) {
      return;
    }
    /// Directly create the string with its amount of #
    std::string str(lvl, '#');
    str += " ";
    str += hdr;
    s << str << std::endl << std::endl;
  }

  void BeginTable(const std::vector<std::string>& head) {
    if (task != 0) {
      return;
    }
    ctc = head.size();
    for (auto& it : head) {
      s << "| " << it << " ";
    }
    s << "|\n";
    for (int i = 0; i < ctc; i++) {
      s << "|---";
    }
    s << "|\n";
    task = 1;
  }

  Markdown& TableAddEntry(const std::string& e) {
    if (task != 1) {
      return *this;
    }
    ctci++;
    s << "| " << e << " ";
    if (ctci == ctc) {
      s << "|\n";
      ctci = 0;
    }
    return *this;
  }

  void EndTable() {
    if (task != 1) {
      return;
    }
    s << std::endl;
    task = 0;
  }

  void Write(const std::string& path) {
    std::ofstream f(path);
    if (!f) {
      return;
    }
    f << s.str();
    f.close();
  }

 private:
  /// @brief Tasks
  /// 0 = free
  /// 1 = table
  /// 2 = text
  int task = 0;
  /// @brief Current Table Columns
  int ctc = 0;
  /// @brief Current Table Column Index
  int ctci = 0;
  std::stringstream s;
};
}  // namespace PD