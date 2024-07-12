#pragma once
#include <string>

namespace Palladium {
/// @brief Format a String
/// @param fmt_str Format To
/// @param ... Additional Args
/// @return Formatted String
std::string FormatString(std::string fmt_str, ...);
/// @brief Get Current Time as String
/// @return Time-String
std::string GetTimeStr(void);
}  // namespace Palladium