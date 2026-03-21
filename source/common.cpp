#include <iostream>
#include <pd/common.hpp>

namespace PD {
constexpr const char* pColorNo = "\033[0m";
constexpr const char* pColorYellow = "\033[33m";
constexpr const char* pColorRed = "\033[31m";
static LogLevel pFilter = LogLevel::Info;

PD_API void LogFilter(LogLevel lvl) {
  pFilter = lvl;
}

PD_API void Log(const std::string& txt, LogLevel lvl) {
  if ((int)lvl < (int)pFilter) return;
  const char* clr = pColorNo;
  const char* plvl = "INFO";
  switch (lvl) {
    case PD::LogLevel::Info:
      clr = pColorNo;
      plvl = "INFO";
      break;
    case PD::LogLevel::Warning:
      clr = pColorYellow;
      plvl = "WARNING";
      break;
    case PD::LogLevel::Error:
      clr = pColorRed;
      plvl = "ERROR";
      break;
  }

  std::cout << clr << "[PD][" << plvl << "] " << txt << pColorNo << std::endl;
}
}  // namespace PD