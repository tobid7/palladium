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

#include <3ds.h>

#include <pd/lib3ds/os.hpp>

namespace PD {
namespace Ctr {
std::string GetSystemLanguage() {
  u8 language = 0;
  Result res = CFGU_GetSystemLanguage(&language);
  if (R_FAILED(res)) {
    return "en";
  }
  switch (language) {
    case 0:
      return "jp";  // Japanese
      break;
    case 1:
      return "en";  // English
      break;
    case 2:
      return "fr";  // French
      break;
    case 3:
      return "de";  // German
      break;
    case 4:
      return "it";  // Italian
      break;
    case 5:
      return "es";  // Spanish
      break;
    case 6:
      return "zh-CN";  // Chinese (Simplified)
      break;
    case 7:
      return "ko";  // Korean
      break;
    case 8:
      return "nl";  // Dutch
      break;
    case 9:
      return "pt";  // Portuguese
      break;
    case 10:
      return "ru";  // Russian
      break;
    case 11:
      return "zh-TW";  // Chinese (Traditional)
      break;
    default:
      return "en";  // Fall back to English if missing
      break;
  }
}
}  // namespace Ctr
}  // namespace PD