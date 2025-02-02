/*
MIT License

Copyright (c) 2024 tobid7

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
#include <pd/common/strings.hpp>

#ifndef PALLADIUM_VERSION
#define PALLADIUM_VERSION "unknown"
#endif
#ifndef PALLADIUM_GIT_COMMIT
#define PALLADIUM_GIT_COMMIT "unknown"
#endif

const std::string PD::LibInfo::CompiledWith() {
  return Strings::GetCompilerVersion();
}
const std::string PD::LibInfo::CxxVersion() {
  return "CPP: " + std::to_string(__cplusplus);
}
const std::string PD::LibInfo::BuildTime() { return __DATE__ " - " __TIME__; }
const std::string PD::LibInfo::Version() { return PALLADIUM_VERSION; }
const std::string PD::LibInfo::Commit() { return PALLADIUM_GIT_COMMIT; }