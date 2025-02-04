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

#include <pd/common/strings.hpp>

namespace PD::Strings {
bool StringEndsWith(const std::string& str,
                    const std::vector<std::string>& exts) {
  // Changed order to not do an substr on empty string
  if (str.empty()) {
    return false;
  } else if (str.substr(0, 2) == "._") {
    return false;
  }
  // Use a more modern way here now
  // to avoid strcasecmp
  if (exts.size() != 0) {
    for (const auto& ext : exts) {
      if (str.substr(str.length() - ext.length()) == ext) {
        return true;
      }
    }
  }
  return false;
}

std::wstring MakeWstring(const std::string& s) {
  // As std::wstring(s.begin(), s.end()); doesn't convert it
  // Normally this should not be done like this but it works
  return std::filesystem::path(s).wstring();
}

const std::string FormatNanos(unsigned long long nanos) {
  // Based on some code of my minecraft plugins
  if (nanos < 1000) {
    return std::format("{}ns", nanos);
  } else if (nanos < 1000000) {
    unsigned long long micros = nanos / 1000;
    return std::format("{}us {}ns", micros, nanos % 1000);
  } else if (nanos < 1000000000) {
    unsigned long long millis = nanos / 1000000;
    return std::format("{}ms {}us", millis, (nanos % 1000000) / 1000);
  } else if (nanos < 60000000000ULL) {
    unsigned long long seconds = nanos / 1000000000;
    return std::format("{}s {}ms", seconds, (nanos % 1000000000) / 1000000);
  } else {
    unsigned long long minutes = nanos / 60000000000ULL;
    unsigned long long seconds = (nanos % 60000000000ULL) / 1000000000;
    return std::format("{}m {}s", minutes, seconds);
  }
  return "";
}

const std::string FormatMillis(unsigned long long millis) {
  // Original Code can be found in some of my mv plugins
  if (millis < 1000) {
    return std::format("{}ms", millis);
  } else if (millis < 60000) {
    unsigned long long seconds = millis / 1000;
    return std::format("{}s {}ms", seconds, (millis % 1000));
  } else {
    unsigned long long minutes = millis / 60000;
    unsigned long long seconds = (millis % 60000) / 1000;
    return std::format("{}m {}s {}ms", minutes, seconds, (millis % 1000));
  }
  return "";
}

const std::string FormatBytes(unsigned long long bytes) {
  static const std::vector<std::string> endings = {
      "B", "KB", "MB", "GB", "TB", "Unk",
  };
  int i = 0;
  double b = bytes;
  while (b > 1024.0) {
    i++;
    b /= 1024;
  }
  if (i >= (int)endings.size()) {
    i = (int)endings.size() - 1;
  }
  return std::format("{:.1f} {}", b, endings[i]);
}

const std::string GetFileName(const std::string& path,
                              const std::string& saperators) {
  auto pos = path.find_last_of(saperators);
  if (pos != path.npos) {
    return path.substr(pos + 1);
  }
  // If No saperator was found return the entire path
  return path;
}

const std::string PathRemoveExtension(const std::string& path) {
  auto pos = path.find_last_of('.');
  if (pos != path.npos) {
    return path.substr(0, pos);
  }
  // If No saperator was found return the entire path
  return path;
}

u32 FastHash(const std::string& s) {
  u32 hash = 5381;
  for (auto& it : s) {
    hash = (hash * 33) + static_cast<u8>(it);
  }
  return hash;
}
}  // namespace PD::Strings