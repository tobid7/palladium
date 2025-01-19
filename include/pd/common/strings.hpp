#pragma once

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

#include <pd/common/common.hpp>

namespace PD {
namespace Strings {
bool StringEndsWith(const std::string& str,
                    const std::vector<std::string>& exts);
std::wstring MakeWstring(const std::string& s);
const std::string FormatNanos(unsigned long long nanos);
const std::string FormatMillis(unsigned long long millis);
const std::string FormatBytes(unsigned long long bytes);
const std::string GetFileName(const std::string& path,
                              const std::string& saperators = "/\\");
const std::string PathRemoveExtension(const std::string& path);
template <typename T>
const std::string ToHex(const T& v);
u32 FastHash(const std::string& s);
}  // namespace Strings
}  // namespace PD