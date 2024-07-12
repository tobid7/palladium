#include <filesystem>
#include <fstream>
#include <memory>
#include <pd/Logger.hpp>
#include <pd/Time.hpp>
#include <pd/palladium.hpp>

namespace Palladium {
LoggerBase::~LoggerBase() {
  if (_log.is_open()) _log.close();
}
void LoggerBase::Init(const std::string& name, bool fileless) {
  if (!fileless) {
    std::string path_base = Palladium::GetAppDirectory() + "/logs/";
    if (!std::filesystem::is_directory(path_base)) {
      std::filesystem::create_directories(path_base);
    }
    auto ts = Palladium::GetTimeStr();
    std::string fn = name + ts + ".txt";
    this->filename = name;
    this->log_path = path_base + name;
    if (std::filesystem::exists(this->log_path)) {
      // Do nothing
    } else {
      _log.open(this->log_path, std::ios::out);
    }
  }
  this->Write("Palladium Log\n\n");
}

void LoggerBase::Write(const std::string& debug_text, int lvl) {
  std::string msg = "[" + Palladium::GetTimeStr() + "]: " + debug_text;
  if (this->_log.is_open() && lvl <= writelvl) {
    this->_log << msg << std::endl;
  }
  /*while (msg.find_first_of('\n') != 0) {
    lines.push_back(msg.substr(0, msg.find_first_of('\n')));
    msg = msg.substr(msg.find_first_of('\n'));
  }*/
  lines.push_back(msg);
}

const std::vector<std::string>& LoggerBase::Lines() { return this->lines; }
}  // namespace Palladium