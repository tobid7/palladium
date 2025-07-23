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

/**
 * Yet another Stupid Code generation tool
 * Why ?
 * Cause this is written and tested faster then
 * manually writeup vec2 to vec4
 */

#include <iostream>
#include <lazyvec.hpp>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << argv[0] << " <num (2 to 4)>" << std::endl;
    return 0;
  }
  int l = std::stoi(argv[1]);
  if (l < 2 || l > 4) {
    std::cout << argv[0] << " <num (2 to 4)>" << std::endl;
    return 0;
  }
  std::ofstream f("vec" + std::to_string(l) + ".hpp");
  f << LVec::GenerateHeader(l);
  f.close();
  return 0;
}