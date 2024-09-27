#include <3ds.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <pd/base/Lang.hpp>

static nlohmann::json appJson;

/// Lang Map    KEY          STRING
static std::map<std::string, std::string> lang_table;

std::string Palladium::Lang::GetSys() {
  u8 language = 1;
  CFGU_GetSystemLanguage(&language);

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
std::string Palladium::Lang::Get(const std::string &key) {
  auto tmp = lang_table.find(key);
  if (tmp == lang_table.end()) return "ERR-02";
  return tmp->second;
}

void Palladium::Lang::Load(const std::string &lang) {
  std::fstream values;
  if (std::filesystem::exists("romfs:/lang/" + lang + "/app.json")) {
    values.open("romfs:/lang/" + lang + "/app.json", std::ios::in);
    if (values.is_open()) {
      appJson = nlohmann::json::parse(values);
    }
    values.close();
    if (appJson.is_discarded()) {
      appJson = {};
    }
  } else {
    values.open("romfs:/lang/en/app.json", std::ios::in);
    if (values.is_open()) {
      appJson = nlohmann::json::parse(values);
    }
    values.close();
    if (appJson.is_discarded()) {
      appJson = {};
    }
  }
  lang_table.clear();
  if (appJson.contains("keys")) {
    for (auto &it : appJson["keys"].items()) {
      lang_table[it.key()] = it.value().get<std::string>();
    }
  }
}

std::string Palladium::Lang::GetName() {
  if (!appJson.contains("info")) return "";
  nlohmann::json js = appJson["info"];
  if (!js.contains("name")) return "Unknown";
  return js.at("name").get<std::string>();
}

std::string Palladium::Lang::GetAuthor() {
  if (!appJson.contains("info")) return "";
  nlohmann::json js = appJson["info"];
  if (!js.contains("author")) return "Unknown";
  return js.at("author").get<std::string>();
}

std::string Palladium::Lang::GetShortcut() {
  if (!appJson.contains("info")) return "";
  nlohmann::json js = appJson["info"];
  if (!js.contains("shortcut")) return "Unknown";
  return js.at("shortcut").get<std::string>();
}