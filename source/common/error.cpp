/*
MIT License
Copyright (c) 2024 René Amthor (tobid7)

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

#include <3ds.h>

#include <pd/common/error.hpp>

namespace PD {
void Error(const std::string& msg) {
  gfxInitDefault();
  consoleInit(GFX_TOP, NULL);
  std::cout << "Palladium - ERROR MANAGER\n" << std::endl;
  std::cout << msg << std::endl << std::endl;
  std::cout << "Press Start to Exit" << std::endl;
  while (aptMainLoop()) {
    hidScanInput();
    if (hidKeysDown() & KEY_START) {
      break;
    }
    gfxSwapBuffers();
  }
  gfxExit();
  exit(0);
}
void Assert(bool v, const std::string& msg) {
  if (v == false) {
    Error("Assert Failed:\n" + msg);
  }
}
}  // namespace PD