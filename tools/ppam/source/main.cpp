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

// C++ 20 capable compiler required (eg. force use
// self compiled clang on debian based systems)
#include <format>
#include <fstream>
#include <iostream>
#include <string>

constexpr std::string_view ppa_text = R"(#pragma once

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

/** Generated with ppam */

#ifdef _WIN32  // Windows (MSVC Tested)
#ifdef {0}_BUILD_SHARED
#define {0}_API __declspec(dllexport)
#else
#define {0}_API __declspec(dllimport)
#endif
#elif defined(__APPLE__)  // macOS (untested yet)
#ifdef {0}_BUILD_SHARED
#define {0}_API __attribute__((visibility("default")))
#else
#define {0}_API
#endif
#elif defined(__linux__)  // Linux (untested yet)
#ifdef {0}_BUILD_SHARED
#define {0}_API __attribute__((visibility("default")))
#else
#define {0}_API
#endif
#elif defined(__3DS__)  // 3ds Specific
// Only Static supported
#define {0}_API
#else
#define {0}_API
#endif
)";

/**
 * Tool to generate the `pd_p_api.hpp` (Palladium Platform Api)
 * for palladiums sub libraries to support dll/so/dlsym
 */

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << argv[0] << " <name>" << std::endl;
    return 0;
  }
  std::fstream off("pd_p_api.hpp", std::ios::out);
  off << std::format(ppa_text, argv[1]);
  off.close();
  return 0;
}