#pragma once

#include <pd/Ovl.hpp>
#include <string>

typedef int PDKeyboard;

enum PDKeyboard_ {
  PDKeyboard_Default,
  PDKeyboard_Numpad,
  PDKeyboard_Password,
};

enum PDKeyboardState {
  PDKeyboardState_None = 0,
  PDKeyboardState_Cancel = 1,
  PDKeyboardState_Confirm = 2,
};

namespace Palladium {
class Ovl_Ftrace : public Palladium::Ovl {
 public:
  /// @brief Constructor
  Ovl_Ftrace(bool* is_enabled);
  /// @brief Override for Draw
  void Draw(void) const override;
  /// @brief Override for Logic
  void Logic() override;

 private:
  bool* i_is_enabled;
};

class Ovl_Metrik : public Palladium::Ovl {
 public:
  /// @brief Constructor
  Ovl_Metrik(bool* is_enabled, bool* screen, uint32_t* mt_color,
             uint32_t* txt_color, float* txt_size);
  /// @brief Override for Draw
  void Draw(void) const override;
  /// @brief Override for Logic
  void Logic() override;

 private:
  // Mutable internal values
  mutable std::string mt_fps;
  mutable std::string mt_cpu;
  mutable std::string mt_gpu;
  mutable std::string mt_cmd;
  mutable std::string mt_lfr;
  mutable std::string mt_vtx;
  mutable std::string mt_drc;
  mutable std::string mt_dmc;
  mutable std::string mt_mem;

  // Importand Adresses
  bool* i_is_enabled;
  bool* i_screen;
  uint32_t* i_mt_color;
  uint32_t* i_txt_color;
  float* i_txt_size;
};

class Ovl_Keyboard : public Palladium::Ovl {
 public:
  /// @brief Constructor
  /// Keyboard Type not Supported for now
  Ovl_Keyboard(std::string& ref, PDKeyboardState& state,
               const std::string& hint = "", PDKeyboard type = 0);
  /// @brief Deconstructor
  ~Ovl_Keyboard();
  /// @brief Override for Draw
  void Draw(void) const override;
  /// @brief Override for Logic
  void Logic() override;

 private:
  mutable std::map<unsigned char, char> shared_data;
  // Pointer to useres String
  std::string* typed_text = nullptr;
  std::string str_bak;
  PDKeyboardState* state;
  PDKeyboard type;
  int mode = 0;
  int ft3 = 0;
};
}  // namespace Palladium