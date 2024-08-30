#pragma once

#include <pd/Ovl.hpp>
#include <pd/Timer.hpp>
#include <pd/base/FunctionTrace.hpp>
#include <pd/maths/NVec.hpp>
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

using PDKeyboardFlags = unsigned int;
enum PDKeyboardFlags_ {
  PDKeyboardFlags_None = 0,
  PDKeyboardFlags_BlendTop = 1 << 0,
  PDKeyboardFlags_BlendBottom = 1 << 1,
  PDKeyboardFlags_LockControls = 1 << 2,
  PDKeyboardFlags_Default =
      PDKeyboardFlags_BlendTop | PDKeyboardFlags_BlendBottom | PDKeyboardFlags_LockControls,
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
  Ovl_Metrik(bool* is_enabled, bool* screen, unsigned int* mt_color,
             unsigned int* txt_color, float* txt_size);
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
  mutable std::string mt_idx;
  mutable std::string mt_drc;
  mutable std::string mt_dmc;
  mutable std::string mt_mem;

  // Importand Adresses
  bool* i_is_enabled;
  bool* i_screen;
  unsigned int* i_mt_color;
  unsigned int* i_txt_color;
  float* i_txt_size;
  mutable Ftrace::TimeStats cpu_stats;
  mutable Ftrace::TimeStats gpu_stats;
  mutable Timer v_update;
};

class Ovl_Keyboard : public Palladium::Ovl {
 public:
  /// @brief Constructor
  /// Keyboard Type not Supported for now
  Ovl_Keyboard(std::string& ref, PDKeyboardState& state,
               const std::string& hint = "", PDKeyboard type = 0,
               PDKeyboardFlags flags = PDKeyboardFlags_Default);
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
  PDKeyboardFlags flags;
};
}  // namespace Palladium