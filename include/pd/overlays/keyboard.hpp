#pragma once

#include <pd/controls/hid.hpp>
#include <pd/maths/tween.hpp>
#include <pd/overlays/overlay.hpp>

namespace PD {
void DumpLayout(const std::string& path);
/// Keyboard class
/// @brief needs to be pushed with text and State reference as Overlay
/// to communicate with it
/// @note Hardcoded Rendering to get maximum Rendering Performance
class Keyboard : public Overlay {
 public:
  enum KeyOperation {
    AppendSelf = 0,
    Shift = 1,
    Backspace = 2,
    Enter = 3,
    OpCancel = 4,
    OpConfirm = 5,
    Tab = 6,
    Caps = 7,
    Space = 8,
    Op1 = 9,
    Op2 = 10,
  };
  enum Type {
    Default,
    Numpad,
    Password,
  };
  enum State {
    None,
    Cancel,
    Confirm,
  };
  using Flags = u32;
  enum Flags_ {
    Flags_None = 0,
    Flags_BlendTop = 1 << 0,
    Flags_BlendBottom = 1 << 1,
    Flags_LockControls = 1 << 2,
    Flags_Default = Flags_BlendBottom | Flags_BlendTop | Flags_LockControls,
  };
  Keyboard(std::string& text, State& state, const std::string& hint = "",
           Type type = Default, Flags flags = Flags_Default) {
    too++;
    if (too > 1) {
      Kill();
      return;
    }
    this->text = &text;
    this->copy = text;
    this->state = &state;
    this->hint = hint;
    this->type = type;
    this->flags = flags;
    this->raw_sel = -1;
    flymgr.From(vec2(0, 240)).To(vec2(0, 115)).In(0.3f).As(flymgr.EaseInQuad);
    chflymgr.From(vec2(-320, 0)).To(vec2(-320, 0)).In(0.1f).As(chflymgr.Linear);
  }
  ~Keyboard() { too--; }

  void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) override;

  void Rem() {
    rem = true;
    flymgr.From(vec2(0, 115)).To(vec2(0, 240)).In(0.2f).As(flymgr.EaseOutQuad);
  }

 private:
  void LoadTheKeys(LI::Renderer::Ref ren);
  void Movement(Hid::Ref inp);
  void MoveSelector();
  void DoOperation(KeyOperation op, const std::string& kname);
  void RecolorBy(KeyOperation op, u32 color, int cm);
  void InputOpBind(Hid::Key k, KeyOperation op, Hid::Ref inp, int cm);
  std::string* text;
  std::string copy;
  std::string hint;
  State* state;
  Type type;
  Flags flags;
  int mode = 0;  // def, caps, shift
  // Stands for The Only One
  static int too;

  Tween<vec2> selector;
  Tween<vec2> sel_szs;
  vec2 cselszs;
  int raw_sel;

  // Performance Optimisation
  LI::StaticObject::Ref keys[3];
  bool keys_loadet = false;

  // Some Animation
  bool rem = false;
  /// Probably a float would've done the job as well ;)
  Tween<vec2> flymgr;
  Tween<vec2> chflymgr;
  bool show_help = true;
};
}  // namespace PD