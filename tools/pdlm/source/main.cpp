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
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>

constexpr std::string_view cmake_file = R"(cmake_minimum_required(VERSION 3.22)

# Generated with pdlm

project({0} LANGUAGES {1} VERSION {2})

set(SRC
    {3}
)

if(PD_BUILD_SHARED)
    pd_add_lib({0} BUILD_SHARED TRUE SRC_FILES ${{SRC}})
else()
    pd_add_lib({0} SRC_FILES ${{SRC}})
endif()
)";

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << argv[0] << " <libname> <lang (in \"\")> <libver>" << std::endl;
    return 0;
  }
  if (std::filesystem::exists("CMakeLists.txt")) {
    std::cout << "!!! CMakeLists.txt in your working directory !!!"
              << std::endl;
    std::cout
        << "!!! Make sure to execute this command in an Empty directory !!!"
        << std::endl;
    return 0;
  }
  std::fstream off("CMakeLists.txt", std::ios::out);
  off << std::format(cmake_file, argv[1], argv[2], argv[3], "your_source.cpp");
  off.close();
  return 0;
}