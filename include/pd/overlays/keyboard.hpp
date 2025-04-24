#pragma once

/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <pd/core/core.hpp>
#include <pd/overlays/overlay.hpp>

namespace PD {
/**
 * Development Function to dump Keyboard Layout into a Json File
 * @param path Path to write into
 */
void DumpLayout(const std::string& path);
/**
 * Keyboard class
 *
 * - needs to be pushed with text and State reference as Overlay
 * to communicate with it
 * @note Hardcoded Rendering to get maximum Rendering Performance
 */
class Keyboard : public Overlay {
 public:
  /** Key Operations */
  enum KeyOperation {
    AppendSelf = 0,  ///< Append Keyname to res string
    Shift = 1,       ///< Shift
    Backspace = 2,   ///< Backspace
    Enter = 3,       ///< Enter
    OpCancel = 4,    ///< Cancel
    OpConfirm = 5,   ///< Confirm
    Tab = 6,         ///< Tab
    Caps = 7,        ///< Caps
    Space = 8,       ///< Space
    Op1 = 9,         ///< Unnset Op 1
    Op2 = 10,        ///< Unset Op 2
  };
  /** Keyboard Type */
  enum Type {
    Default,   ///< Default Keyboard
    Numpad,    ///< Numpad
    Password,  ///< Password Input
  };
  /** State */
  enum State {
    None,     ///< Doing nothing
    Cancel,   ///< Cancelled
    Confirm,  ///< Confirmed
  };
  /** Alias for Keyboard Flags */
  using Flags = u32;
  /** Flags */
  enum Flags_ {
    Flags_None = 0,               ///< Nothing
    Flags_BlendTop = 1 << 0,      ///< Blend Top Screen
    Flags_BlendBottom = 1 << 1,   ///< Blend Bottom
    Flags_LockControls = 1 << 2,  ///< Lock Contols (Input Driver)
    Flags_Transparency = 1 << 3,  ///< Transparant Keyboard Colors
    // Default Flags
    Flags_Default = Flags_BlendBottom | Flags_BlendTop | Flags_LockControls,
  };
  /**
   * Keyboard Constructor
   * @param text Result Text
   * @param state Result State
   * @param hint Hint to display if result is empty
   * @param type Keyboard type
   * @param flags Keyboard flags to use
   */
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
  /** Deconstructor */
  ~Keyboard() { too--; }

  /**
   * Rendering and Input Handler
   * @param delta Deltatime for Animations
   * @param ren Renderer Reference
   * @param inp Input Driver reference
   */
  void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) override;

  /** Remove Keyboard */
  void Rem() {
    rem = true;
    flymgr.From(vec2(0, 115)).To(vec2(0, 240)).In(0.2f).As(flymgr.EaseOutQuad);
  }

 private:
  /** Prerender Keys */
  void LoadTheKeys(LI::Renderer::Ref ren);
  /** Controller Movement */
  void Movement(Hid::Ref inp);
  /** Trigger Move from to Animation */
  void MoveSelector();
  /** Execute a Key operation */
  void DoOperation(KeyOperation op, const std::string& kname);
  /** Recolor all Keys with the same operation */
  void RecolorBy(KeyOperation op, u32 color, int cm);
  /** Bind an operation */
  void InputOpBind(Hid::Key k, KeyOperation op, Hid::Ref inp, int cm);
  // Result Text reference
  std::string* text;
  // Copy of the Text
  std::string copy;
  // Hint
  std::string hint;
  // Result State reference
  State* state;
  // Keyboard Type
  Type type;
  // Keyboard flags
  Flags flags;
  // def, caps, shift
  int mode = 0;
  // Stands for The Only One
  static int too;

  // Tween for selector Movement
  Tween<fvec2> selector;
  // Tween for Selector Size
  Tween<fvec2> sel_szs;
  // Current Cell size
  fvec2 cselszs;
  // selector index
  int raw_sel;

  // Prerendered Keytables
  LI::StaticObject::Ref keys[3];
  // Value to check if table is loadet
  bool keys_loadet = false;

  // Remove Animation
  bool rem = false;
  /// Probably a float would've done the job as well ;)
  Tween<fvec2> flymgr;
  // Secode Flymanager
  Tween<fvec2> chflymgr;
  // Show Help
  bool show_help = true;
};
}  // namespace PD