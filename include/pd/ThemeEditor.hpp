#pragma once

#include <pd/palladium.hpp>

namespace Palladium {
class ThemeEditor : public Palladium::Scene {
 public:
  ThemeEditor();
  ~ThemeEditor();

  void Update() override;

 private:
  Theme::Ref edit_theme;
  // Placeholder to save active one to
  Theme::Ref temp_theme;

  // temp vars for samples
  bool cm;
  std::string inpt;
  int menu = 0;

  // Keyboard
  PDKeyboardState kbd_state;
  std::string kbd_text;
  std::vector<std::string> theme_list;
};
}  // namespace Palladium