#pragma once
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace Palladium {
/// @brief Check if A String ends with
/// @param name Input String
/// @param extensions Extensions to Check for
/// @return Ends with or not
inline bool NameIsEndingWith(const std::string &name,
                             const std::vector<std::string> &extensions) {
  if (name.substr(0, 2) == "._") return false;

  if (name.size() == 0) return false;

  if (extensions.size() == 0) return true;

  for (int i = 0; i < (int)extensions.size(); i++) {
    const std::string ext = extensions.at(i);
    if (strcasecmp(name.c_str() + name.size() - ext.size(), ext.c_str()) == 0)
      return true;
  }

  return false;
}
/// @brief Format Milliseconds to clean string (Stolen from one of my Mc
/// Plugins)
/// @param t_time Time in ms
/// @return String
inline std::string MsTimeFmt(float t_time, bool dems = false) {
  std::string res;

  if (t_time < 0.000001f) {
    res = std::format("{:.2f}ns", t_time * 1000000.f);
  } else if (t_time < 0.001f) {
    res = std::format("{:.2f}µs", t_time * 1000.f); 
  } else if (t_time < 1.0f) {
    res = std::format("{:.2f}ms", t_time);
  } else if (t_time < 60000.0f) {
    int seconds = static_cast<int>(t_time / 1000.0f);
    float milliseconds = t_time - (seconds * 1000.0f);
    if (seconds) {
      res = std::format("{}s {:.2f}ms", seconds, milliseconds);
    }
    res = std::format("{:.2f}ms", milliseconds);
  } else {
    int minutes = static_cast<int>(t_time / 60000.0f);
    int seconds = static_cast<int>((t_time - (minutes * 60000.0f)) / 1000.0f);
    float milliseconds = t_time - (minutes * 60000.0f) - (seconds * 1000.0f);

    res = std::format("{}m {}s {:.2f}ms", minutes, seconds, milliseconds);
  }

  return res;
}

inline std::string FormatBytes(int bytes) {
  char out[32];

  if (bytes == 1)
    snprintf(out, sizeof(out), "%d Byte", bytes);

  else if (bytes < 1024)
    snprintf(out, sizeof(out), "%d Bytes", bytes);

  else if (bytes < 1024 * 1024)
    snprintf(out, sizeof(out), "%.1f KB", (float)bytes / 1024);

  else if (bytes < 1024 * 1024 * 1024)
    snprintf(out, sizeof(out), "%.1f MB", (float)bytes / 1024 / 1024);

  else
    snprintf(out, sizeof(out), "%.1f GB", (float)bytes / 1024 / 1024 / 1024);

  return out;
}
}  // namespace Palladium

template <class T>
T GetFileName(T const &path, T const &delims = "/\\") {
  return path.substr(path.find_last_of(delims) + 1);
}
template <class T>
T remove_ext(T const &filename) {
  typename T::size_type const p(filename.find_last_of('.'));
  return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}

template <typename T>
std::string Int_To_Hex(T i) {
  std::stringstream stream;
  stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex
         << i;
  return stream.str();
}