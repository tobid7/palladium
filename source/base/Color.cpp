#include <filesystem>
#include <fstream>
#include <map>
#include <pd/base/Color.hpp>
#include <pd/Message.hpp>
#include <pd/external/json.hpp>
#include <pd/internal_db.hpp>

void pdi_swap32(unsigned int& c) {
  c = ((c & 0xFF) << 24) | ((c & 0xFF00) << 8) | ((c & 0xFF0000) >> 8) |
      ((c & 0xFF000000) >> 24);
}

std::string Palladium::Color::RGBA2Hex(unsigned int c32) {
  pdi_swap32(c32);
  std::stringstream ss;
  ss << "#";
  ss << std::hex << std::setw(8) << std::setfill('0') << c32;
  return ss.str();
}

// Standart Color Converter
static const std::map<char, int> HEX_TO_DEC = {
    {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
    {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'a', 10}, {'b', 11},
    {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}};

unsigned int pdi_special_color_hex(const std::string& hex) {
  if (hex.length() < 9 || std::find_if(hex.begin() + 1, hex.end(), [](char c) {
                            return !std::isxdigit(c);
                          }) != hex.end()) {
    return pdi_special_color_hex("#00000000");
  }

  int r = HEX_TO_DEC.at(hex[1]) * 16 + HEX_TO_DEC.at(hex[2]);
  int g = HEX_TO_DEC.at(hex[3]) * 16 + HEX_TO_DEC.at(hex[4]);
  int b = HEX_TO_DEC.at(hex[5]) * 16 + HEX_TO_DEC.at(hex[6]);
  int a = HEX_TO_DEC.at(hex[7]) * 16 + HEX_TO_DEC.at(hex[8]);

  return RGBA8(r, g, b, a);
}

// Default Theme
const std::map<PDColor, unsigned int> pdi_default_theme = {
    {PDColor_Text, RGBA8(0, 0, 0, 255)},
    {PDColor_Text2, RGBA8(255, 255, 255, 255)},  // For Background change or so
    {PDColor_TextDisabled, RGBA8(170, 170, 170, 255)},
    {PDColor_Background, RGBA8(238, 238, 238, 255)},
    {PDColor_Header, RGBA8(17, 17, 17, 255)},
    {PDColor_Selector, RGBA8(34, 34, 34, 255)},
    {PDColor_SelectorFade, RGBA8(90, 90, 90, 255)},
    {PDColor_List0, RGBA8(204, 204, 204, 255)},  // List0 = % 2
    {PDColor_List1, RGBA8(187, 187, 187, 255)},
    {PDColor_MessageBackground, RGBA8(51, 51, 51, 255)},
    {PDColor_Button, RGBA8(17, 17, 17, 255)},
    {PDColor_ButtonHovered, RGBA8(34, 34, 34, 255)},
    {PDColor_ButtonDisabled, RGBA8(8, 8, 8, 255)},
    {PDColor_ButtonActive, RGBA8(42, 42, 42, 255)},
    {PDColor_Checkmark, RGBA8(42, 42, 42, 255)},
    {PDColor_FrameBg, RGBA8(85, 85, 85, 255)},
    {PDColor_FrameBgHovered, RGBA8(119, 119, 119, 255)},
    {PDColor_Progressbar, RGBA8(0, 255, 0, 255)},
};

void Palladium::Theme::Load(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return;
  }
  nlohmann::json js;
  file >> js;
  // clang-format off
  if(THEMEVER != js["version"]) {
    file.close();
    return; 
  }
  this->clr_tab.clear();
  this->clr_tab.resize(PDColor_Len);
  this->clr_tab[PDColor_Text] = pdi_special_color_hex(js["PDColor_Text"].get<std::string>());
  this->clr_tab[PDColor_Text2] = pdi_special_color_hex(js["PDColor_Text2"].get<std::string>());
  this->clr_tab[PDColor_TextDisabled] = pdi_special_color_hex(js["PDColor_TextDisabled"].get<std::string>());
  this->clr_tab[PDColor_Background] = pdi_special_color_hex(js["PDColor_Background"].get<std::string>());
  this->clr_tab[PDColor_Header] = pdi_special_color_hex(js["PDColor_Header"].get<std::string>());
  this->clr_tab[PDColor_Selector] = pdi_special_color_hex(js["PDColor_Selector"].get<std::string>());
  this->clr_tab[PDColor_SelectorFade] = pdi_special_color_hex(js["PDColor_SelectorFade"].get<std::string>());
  this->clr_tab[PDColor_List0] = pdi_special_color_hex(js["PDColor_List0"].get<std::string>());
  this->clr_tab[PDColor_List1] = pdi_special_color_hex(js["PDColor_List1"].get<std::string>());
  this->clr_tab[PDColor_MessageBackground] =  pdi_special_color_hex(js["PDColor_MessageBackground"].get<std::string>());
  this->clr_tab[PDColor_Button] = pdi_special_color_hex(js["PDColor_Button"].get<std::string>());
  this->clr_tab[PDColor_ButtonHovered] = pdi_special_color_hex(js["PDColor_ButtonHovered"].get<std::string>());
  this->clr_tab[PDColor_ButtonDisabled] = pdi_special_color_hex(js["PDColor_ButtonDisabled"].get<std::string>());
  this->clr_tab[PDColor_ButtonActive] = pdi_special_color_hex(js["PDColor_ButtonActive"].get<std::string>());
  this->clr_tab[PDColor_Checkmark] = pdi_special_color_hex(js["PDColor_Checkmark"].get<std::string>());
  this->clr_tab[PDColor_FrameBg] = pdi_special_color_hex(js["PDColor_FrameBg"].get<std::string>());
  this->clr_tab[PDColor_FrameBgHovered] = pdi_special_color_hex(js["PDColor_FrameBgHovered"].get<std::string>());
  this->clr_tab[PDColor_Progressbar] = pdi_special_color_hex(js["PDColor_Progressbar"].get<std::string>());
  // clang-format on
  file.close();
}

void Palladium::Theme::Default() {
  this->clr_tab.clear();
  this->clr_tab.resize(PDColor_Len);
  for (auto& it : pdi_default_theme) {
    this->clr_tab[it.first] = it.second;
  }
}

void Palladium::Theme::CopyOther(Theme::Ref theme) {
  this->clr_tab.clear();
  this->clr_tab.resize(PDColor_Len);
  for (int i = 0; i < (int)PDColor_Len; i++) {
    this->clr_tab[i] = theme->Get(i);
  }
}

unsigned int Palladium::Theme::Get(PDColor clr) {
  if (clr < 0 || clr >= PDColor_Len) return 0;
  return this->clr_tab[clr];
}

void Palladium::Theme::Set(PDColor clr, unsigned int v) {
  if (clr < 0 || clr >= PDColor_Len) return;
  this->changes.push_back(change(clr, this->clr_tab[clr], v));
  this->clr_tab[clr] = v;
}
void Palladium::Theme::Swap(PDColor a, PDColor b) {
  if (a < 0 || a >= PDColor_Len || b < 0 || b >= PDColor_Len) return;
  auto c = this->clr_tab[a];
  this->clr_tab[a] = this->clr_tab[b];
  this->clr_tab[b] = c;
  this->changes.push_back(change(a, b, c, this->clr_tab[a]));
}

void Palladium::Theme::TextBy(PDColor bg) {
  if (!Color::RGBA(bg).is_light()) Swap(PDColor_Text, PDColor_Text2);
}

PDColor Palladium::Theme::AutoText(PDColor bg) {
  return Color::RGBA(bg).is_light() ? PDColor_Text : PDColor_Text2;
}

bool Palladium::Theme::Undo() {
  if (!this->changes.size()) return false;
  auto ch = this->changes[this->changes.size() - 1];
  this->changes.pop_back();
  if (ch.clr2) {
    this->clr_tab[ch.clr2] = ch.to;
    this->clr_tab[ch.clr] = ch.from;
  } else {
    this->clr_tab[ch.clr] = ch.from;
  }
  return true;
}

void Palladium::Theme::UndoAll() {
  while (Undo()) {
    // Just Run Undo Until all is undone
  }
}

void Palladium::Theme::Save(const std::string& path) {
  if (std::filesystem::path(path).filename().string() == "Palladium.theme") {
    if (!pdi_amdt) {
      Palladium::PushMessage("Theme", "Default Theme cannot\nbe overwritten!");
      return;
    }
  }
  std::ofstream file(path);
  if (!file.is_open()) {
    Palladium::PushMessage("Theme", "Unable to\ncreate file!");
    return;
  }
  nlohmann::json js;
  // clang-format off
  js["version"] = THEMEVER;
  js["PDColor_Text"] = Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_Text]);
  js["PDColor_Text2"] = Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_Text2]);
  js["PDColor_TextDisabled"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_TextDisabled]); js["PDColor_Background"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_Background]); js["PDColor_Header"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_Header]); js["PDColor_Selector"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_Selector]); js["PDColor_SelectorFade"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_SelectorFade]); js["PDColor_List0"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_List0]); js["PDColor_List1"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_List1]); js["PDColor_MessageBackground"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_MessageBackground]); js["PDColor_Button"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_Button]); js["PDColor_ButtonHovered"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_ButtonHovered]);
  js["PDColor_ButtonDisabled"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_ButtonDisabled]);
  js["PDColor_ButtonActive"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_ButtonActive]); js["PDColor_Checkmark"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_Checkmark]); js["PDColor_FrameBg"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_FrameBg]); js["PDColor_FrameBgHovered"] =
Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_FrameBgHovered]); js["PDColor_Progressbar"]
= Palladium::Color::RGBA2Hex(this->clr_tab[PDColor_Progressbar]);
  // clang-format on
  file << js.dump(4);
  file.close();
}

Palladium::Theme::Ref Palladium::ThemeActive() { return pdi_active_theme; }

void Palladium::ThemeSet(Palladium::Theme::Ref theme) {
  pdi_active_theme = theme;
}

unsigned int Palladium::Color::Hex(const std::string& color, uint8_t a) {
  if (color.length() < 7 ||
      std::find_if(color.begin() + 1, color.end(),
                   [](char c) { return !std::isxdigit(c); }) != color.end()) {
    return Palladium::Color::Hex("#000000", 0);
  }

  int r = HEX_TO_DEC.at(color[1]) * 16 + HEX_TO_DEC.at(color[2]);
  int g = HEX_TO_DEC.at(color[3]) * 16 + HEX_TO_DEC.at(color[4]);
  int b = HEX_TO_DEC.at(color[5]) * 16 + HEX_TO_DEC.at(color[6]);

  return RGBA8(r, g, b, a);
}

std::string Palladium::Color::RGB2Hex(int r, int g, int b) {
  std::stringstream ss;
  ss << "#";
  ss << std::hex << (r << 16 | g << 8 | b);
  return ss.str();
}