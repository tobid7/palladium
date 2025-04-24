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

#include <pd/external/json.hpp>
#include <pd/overlays/keyboard.hpp>
// #include <pd/lib3ds/gamepad_icons.hpp>

namespace PD {
struct Key {
  Key(const std::string& key, const fvec2& p, const fvec2& s,
      Keyboard::KeyOperation o) {
    k = key;
    pos = p;
    size = s;
    op = o;
  }
  std::string k;
  fvec2 pos;
  fvec2 size;
  Keyboard::KeyOperation op;
};

using Layout = std::vector<Key>;
Layout layouts[3] = {
    {
        // 1st row
        Key("`", fvec2(5, 0), 18, Keyboard::AppendSelf),
        Key("1", fvec2(25, 0), 18, Keyboard::AppendSelf),
        Key("2", fvec2(45, 0), 18, Keyboard::AppendSelf),
        Key("3", fvec2(65, 0), 18, Keyboard::AppendSelf),
        Key("4", fvec2(85, 0), 18, Keyboard::AppendSelf),
        Key("5", fvec2(105, 0), 18, Keyboard::AppendSelf),
        Key("6", fvec2(125, 0), 18, Keyboard::AppendSelf),
        Key("7", fvec2(145, 0), 18, Keyboard::AppendSelf),
        Key("8", fvec2(165, 0), 18, Keyboard::AppendSelf),
        Key("9", fvec2(185, 0), 18, Keyboard::AppendSelf),
        Key("0", fvec2(205, 0), 18, Keyboard::AppendSelf),
        Key("-", fvec2(225, 0), 18, Keyboard::AppendSelf),
        Key("=", fvec2(245, 0), 18, Keyboard::AppendSelf),
        Key("<---", fvec2(265, 0), fvec2(50, 18), Keyboard::Backspace),
        // 2nd row
        Key("Tab", fvec2(5, 20), fvec2(40, 18), Keyboard::Tab),
        Key("q", fvec2(47, 20), 18, Keyboard::AppendSelf),
        Key("w", fvec2(67, 20), 18, Keyboard::AppendSelf),
        Key("e", fvec2(87, 20), 18, Keyboard::AppendSelf),
        Key("r", fvec2(107, 20), 18, Keyboard::AppendSelf),
        Key("t", fvec2(127, 20), 18, Keyboard::AppendSelf),
        Key("y", fvec2(147, 20), 18, Keyboard::AppendSelf),
        Key("u", fvec2(167, 20), 18, Keyboard::AppendSelf),
        Key("i", fvec2(187, 20), 18, Keyboard::AppendSelf),
        Key("o", fvec2(207, 20), 18, Keyboard::AppendSelf),
        Key("p", fvec2(227, 20), 18, Keyboard::AppendSelf),
        Key("[", fvec2(247, 20), 18, Keyboard::AppendSelf),
        Key("]", fvec2(267, 20), 18, Keyboard::AppendSelf),
        Key("\\", fvec2(287, 20), fvec2(28, 18), Keyboard::AppendSelf),
        // 3rd row
        Key("Caps", fvec2(5, 40), fvec2(50, 18), Keyboard::Caps),
        Key("a", fvec2(57, 40), 18, Keyboard::AppendSelf),
        Key("s", fvec2(77, 40), 18, Keyboard::AppendSelf),
        Key("d", fvec2(97, 40), 18, Keyboard::AppendSelf),
        Key("f", fvec2(117, 40), 18, Keyboard::AppendSelf),
        Key("g", fvec2(137, 40), 18, Keyboard::AppendSelf),
        Key("h", fvec2(157, 40), 18, Keyboard::AppendSelf),
        Key("j", fvec2(177, 40), 18, Keyboard::AppendSelf),
        Key("k", fvec2(197, 40), 18, Keyboard::AppendSelf),
        Key("l", fvec2(217, 40), 18, Keyboard::AppendSelf),
        Key(";", fvec2(237, 40), 18, Keyboard::AppendSelf),
        Key("'", fvec2(257, 40), 18, Keyboard::AppendSelf),
        Key("Enter", fvec2(277, 40), fvec2(38, 18), Keyboard::Enter),
        // 4th row
        Key("Shift", fvec2(5, 60), fvec2(60, 18), Keyboard::Shift),
        Key("z", fvec2(67, 60), 18, Keyboard::AppendSelf),
        Key("x", fvec2(87, 60), 18, Keyboard::AppendSelf),
        Key("c", fvec2(107, 60), 18, Keyboard::AppendSelf),
        Key("v", fvec2(127, 60), 18, Keyboard::AppendSelf),
        Key("b", fvec2(147, 60), 18, Keyboard::AppendSelf),
        Key("n", fvec2(167, 60), 18, Keyboard::AppendSelf),
        Key("m", fvec2(187, 60), 18, Keyboard::AppendSelf),
        Key(",", fvec2(207, 60), 18, Keyboard::AppendSelf),
        Key(".", fvec2(227, 60), 18, Keyboard::AppendSelf),
        Key("/", fvec2(247, 60), 18, Keyboard::AppendSelf),
        Key("Shift", fvec2(267, 60), fvec2(48, 18), Keyboard::Shift),
        // 5th row
        Key("Cancel", fvec2(5, 80), fvec2(70, 18), Keyboard::OpCancel),
        Key("(X)", fvec2(77, 80), fvec2(23, 18), Keyboard::Op1),
        Key("Space", fvec2(102, 80), fvec2(108, 18), Keyboard::Space),
        Key("(!)", fvec2(212, 80), fvec2(23, 18), Keyboard::Op2),
        Key("Confirm", fvec2(237, 80), fvec2(78, 18), Keyboard::OpConfirm),
    },
    {
        // 1st row
        Key("`", fvec2(5, 0), 18, Keyboard::AppendSelf),
        Key("1", fvec2(25, 0), 18, Keyboard::AppendSelf),
        Key("2", fvec2(45, 0), 18, Keyboard::AppendSelf),
        Key("3", fvec2(65, 0), 18, Keyboard::AppendSelf),
        Key("4", fvec2(85, 0), 18, Keyboard::AppendSelf),
        Key("5", fvec2(105, 0), 18, Keyboard::AppendSelf),
        Key("6", fvec2(125, 0), 18, Keyboard::AppendSelf),
        Key("7", fvec2(145, 0), 18, Keyboard::AppendSelf),
        Key("8", fvec2(165, 0), 18, Keyboard::AppendSelf),
        Key("9", fvec2(185, 0), 18, Keyboard::AppendSelf),
        Key("0", fvec2(205, 0), 18, Keyboard::AppendSelf),
        Key("-", fvec2(225, 0), 18, Keyboard::AppendSelf),
        Key("=", fvec2(245, 0), 18, Keyboard::AppendSelf),
        Key("<---", fvec2(265, 0), fvec2(50, 18), Keyboard::Backspace),
        // 2nd row
        Key("Tab", fvec2(5, 20), fvec2(40, 18), Keyboard::Tab),
        Key("Q", fvec2(47, 20), 18, Keyboard::AppendSelf),
        Key("W", fvec2(67, 20), 18, Keyboard::AppendSelf),
        Key("E", fvec2(87, 20), 18, Keyboard::AppendSelf),
        Key("R", fvec2(107, 20), 18, Keyboard::AppendSelf),
        Key("T", fvec2(127, 20), 18, Keyboard::AppendSelf),
        Key("Y", fvec2(147, 20), 18, Keyboard::AppendSelf),
        Key("U", fvec2(167, 20), 18, Keyboard::AppendSelf),
        Key("I", fvec2(187, 20), 18, Keyboard::AppendSelf),
        Key("O", fvec2(207, 20), 18, Keyboard::AppendSelf),
        Key("P", fvec2(227, 20), 18, Keyboard::AppendSelf),
        Key("[", fvec2(247, 20), 18, Keyboard::AppendSelf),
        Key("]", fvec2(267, 20), 18, Keyboard::AppendSelf),
        Key("\\", fvec2(287, 20), fvec2(28, 18), Keyboard::AppendSelf),
        // 3rd row
        Key("Caps", fvec2(5, 40), fvec2(50, 18), Keyboard::Caps),
        Key("A", fvec2(57, 40), 18, Keyboard::AppendSelf),
        Key("S", fvec2(77, 40), 18, Keyboard::AppendSelf),
        Key("D", fvec2(97, 40), 18, Keyboard::AppendSelf),
        Key("F", fvec2(117, 40), 18, Keyboard::AppendSelf),
        Key("G", fvec2(137, 40), 18, Keyboard::AppendSelf),
        Key("H", fvec2(157, 40), 18, Keyboard::AppendSelf),
        Key("J", fvec2(177, 40), 18, Keyboard::AppendSelf),
        Key("K", fvec2(197, 40), 18, Keyboard::AppendSelf),
        Key("L", fvec2(217, 40), 18, Keyboard::AppendSelf),
        Key(";", fvec2(237, 40), 18, Keyboard::AppendSelf),
        Key("'", fvec2(257, 40), 18, Keyboard::AppendSelf),
        Key("Enter", fvec2(277, 40), fvec2(38, 18), Keyboard::Enter),
        // 4th row
        Key("Shift", fvec2(5, 60), fvec2(60, 18), Keyboard::Shift),
        Key("Z", fvec2(67, 60), 18, Keyboard::AppendSelf),
        Key("X", fvec2(87, 60), 18, Keyboard::AppendSelf),
        Key("C", fvec2(107, 60), 18, Keyboard::AppendSelf),
        Key("V", fvec2(127, 60), 18, Keyboard::AppendSelf),
        Key("B", fvec2(147, 60), 18, Keyboard::AppendSelf),
        Key("N", fvec2(167, 60), 18, Keyboard::AppendSelf),
        Key("M", fvec2(187, 60), 18, Keyboard::AppendSelf),
        Key(",", fvec2(207, 60), 18, Keyboard::AppendSelf),
        Key(".", fvec2(227, 60), 18, Keyboard::AppendSelf),
        Key("/", fvec2(247, 60), 18, Keyboard::AppendSelf),
        Key("Shift", fvec2(267, 60), fvec2(48, 18), Keyboard::Shift),
        // 5th row
        Key("Cancel", fvec2(5, 80), fvec2(70, 18), Keyboard::OpCancel),
        Key("(X)", fvec2(77, 80), fvec2(23, 18), Keyboard::Op1),
        Key("Space", fvec2(102, 80), fvec2(108, 18), Keyboard::Space),
        Key("(!)", fvec2(212, 80), fvec2(23, 18), Keyboard::Op2),
        Key("Confirm", fvec2(237, 80), fvec2(78, 18), Keyboard::OpConfirm),
    },
    {
        // 1st row
        Key("~", fvec2(5, 0), 18, Keyboard::AppendSelf),
        Key("!", fvec2(25, 0), 18, Keyboard::AppendSelf),
        Key("@", fvec2(45, 0), 18, Keyboard::AppendSelf),
        Key("#", fvec2(65, 0), 18, Keyboard::AppendSelf),
        Key("$", fvec2(85, 0), 18, Keyboard::AppendSelf),
        Key("%", fvec2(105, 0), 18, Keyboard::AppendSelf),
        Key("^", fvec2(125, 0), 18, Keyboard::AppendSelf),
        Key("&", fvec2(145, 0), 18, Keyboard::AppendSelf),
        Key("*", fvec2(165, 0), 18, Keyboard::AppendSelf),
        Key("(", fvec2(185, 0), 18, Keyboard::AppendSelf),
        Key(")", fvec2(205, 0), 18, Keyboard::AppendSelf),
        Key("_", fvec2(225, 0), 18, Keyboard::AppendSelf),
        Key("+", fvec2(245, 0), 18, Keyboard::AppendSelf),
        Key("<---", fvec2(265, 0), fvec2(50, 18), Keyboard::Backspace),
        // 2nd row
        Key("Tab", fvec2(5, 20), fvec2(40, 18), Keyboard::Tab),
        Key("Q", fvec2(47, 20), 18, Keyboard::AppendSelf),
        Key("W", fvec2(67, 20), 18, Keyboard::AppendSelf),
        Key("E", fvec2(87, 20), 18, Keyboard::AppendSelf),
        Key("R", fvec2(107, 20), 18, Keyboard::AppendSelf),
        Key("T", fvec2(127, 20), 18, Keyboard::AppendSelf),
        Key("Y", fvec2(147, 20), 18, Keyboard::AppendSelf),
        Key("U", fvec2(167, 20), 18, Keyboard::AppendSelf),
        Key("I", fvec2(187, 20), 18, Keyboard::AppendSelf),
        Key("O", fvec2(207, 20), 18, Keyboard::AppendSelf),
        Key("P", fvec2(227, 20), 18, Keyboard::AppendSelf),
        Key("{", fvec2(247, 20), 18, Keyboard::AppendSelf),
        Key("}", fvec2(267, 20), 18, Keyboard::AppendSelf),
        Key("|", fvec2(287, 20), fvec2(28, 18), Keyboard::AppendSelf),
        // 3rd row
        Key("Caps", fvec2(5, 40), fvec2(50, 18), Keyboard::Caps),
        Key("A", fvec2(57, 40), 18, Keyboard::AppendSelf),
        Key("S", fvec2(77, 40), 18, Keyboard::AppendSelf),
        Key("D", fvec2(97, 40), 18, Keyboard::AppendSelf),
        Key("F", fvec2(117, 40), 18, Keyboard::AppendSelf),
        Key("G", fvec2(137, 40), 18, Keyboard::AppendSelf),
        Key("H", fvec2(157, 40), 18, Keyboard::AppendSelf),
        Key("J", fvec2(177, 40), 18, Keyboard::AppendSelf),
        Key("K", fvec2(197, 40), 18, Keyboard::AppendSelf),
        Key("L", fvec2(217, 40), 18, Keyboard::AppendSelf),
        Key(":", fvec2(237, 40), 18, Keyboard::AppendSelf),
        Key("\"", fvec2(257, 40), 18, Keyboard::AppendSelf),
        Key("Enter", fvec2(277, 40), fvec2(38, 18), Keyboard::Enter),
        // 4th row
        Key("Shift", fvec2(5, 60), fvec2(60, 18), Keyboard::Shift),
        Key("Z", fvec2(67, 60), 18, Keyboard::AppendSelf),
        Key("X", fvec2(87, 60), 18, Keyboard::AppendSelf),
        Key("C", fvec2(107, 60), 18, Keyboard::AppendSelf),
        Key("V", fvec2(127, 60), 18, Keyboard::AppendSelf),
        Key("B", fvec2(147, 60), 18, Keyboard::AppendSelf),
        Key("N", fvec2(167, 60), 18, Keyboard::AppendSelf),
        Key("M", fvec2(187, 60), 18, Keyboard::AppendSelf),
        Key("<", fvec2(207, 60), 18, Keyboard::AppendSelf),
        Key(">", fvec2(227, 60), 18, Keyboard::AppendSelf),
        Key("?", fvec2(247, 60), 18, Keyboard::AppendSelf),
        Key("Shift", fvec2(267, 60), fvec2(48, 18), Keyboard::Shift),
        // 5th row
        Key("Cancel", fvec2(5, 80), fvec2(70, 18), Keyboard::OpCancel),
        Key("(X)", fvec2(77, 80), fvec2(23, 18), Keyboard::Op1),
        Key("Space", fvec2(102, 80), fvec2(108, 18), Keyboard::Space),
        Key("(!)", fvec2(212, 80), fvec2(23, 18), Keyboard::Op2),
        Key("Confirm", fvec2(237, 80), fvec2(78, 18), Keyboard::OpConfirm),
    },
};

void DumpLayout(const std::string& path) {
  nlohmann::json l0;
  l0["name"] = "Default US";
  for (int i = 0; i < 3; i++) {
    nlohmann::json l1;
    for (size_t j = 0; j < layouts[0].size(); j++) {
      nlohmann::json key;
      key["display_char"] = layouts[i][j].k;
      key["pos_x"] = layouts[i][j].pos.x;
      key["pos_y"] = layouts[i][j].pos.y;
      key["size_x"] = layouts[i][j].size.x;
      key["size_y"] = layouts[i][j].size.y;
      key["op"] = layouts[i][j].op;
      l1.push_back(key);
    }
    l0[std::to_string(i)] = l1;
  }
  std::ofstream off(path);
  off << l0.dump(3);
  off.close();
}

/// The Only One (too) is a static var to make sur
/// THe Keyboard can only exist once in the overlay mgr
int Keyboard::too = 0;

void Keyboard::MoveSelector() {
  /// Move from Current position to New Position
  selector.From(selector).To(layouts[0][raw_sel].pos).In(0.1f);
  /// If Button Size Changed, animate to the new size
  if (cselszs != layouts[0][raw_sel].size) {
    cselszs = layouts[0][raw_sel].size;
    sel_szs.Swap();
    sel_szs.To(cselszs).In(0.1);
  }
}

void Keyboard::LoadTheKeys(LI::Renderer::Ref ren) {
  for (int i = 0; i < 3; i++) {
    keys[i] = LI::StaticObject::New();
    for (auto it : layouts[i]) {
      vec2 pos = it.pos + it.size * 0.5 - ren->GetTextDimensions(it.k) * 0.5;
      auto c = LI::Command::New();
      auto r = ren->CreateRect(it.pos, it.size, 0.f);
      int l = ren->Layer();
      ren->UseTex();
      ren->Layer(1);
      ren->SetupCommand(c);
      ren->QuadCommand(c, r, vec4(0.f, 1.f, 1.f, 0.f), 0xff444444);
      keys[i]->PushCommand(c);
      ren->Layer(2);
      ren->TextCommand(keys[i]->List(), pos, 0xffffffff, it.k, 0, 0);
      ren->Layer(l);
    }
    ren->OptiCommandList(keys[i]->List());
  }
  keys_loadet = true;
}

void Keyboard::Movement(Hid::Ref inp) {
  /// Any Key if no selector
  if (raw_sel < 0) {
    /// Initial Selector PopUp
    if (inp->IsUp((Hid::Key)(inp->Up | inp->Down | inp->Left | inp->Right))) {
      raw_sel = 0;
      vec2 dst = layouts[0][0].pos;
      cselszs = layouts[0][0].size;
      selector.As(selector.Linear).From(dst + (cselszs * 0.5)).To(dst).In(0.1f);
      sel_szs.As(sel_szs.Linear).From(0).To(cselszs).In(0.1f);
    }
  } else {
    /// Go Up Movement
    if (inp->IsUp(inp->Up)) {
      vec2 tpos = layouts[0][raw_sel].pos;
      vec2 tsize = layouts[0][raw_sel].size;
      float tcen = tpos.x() + (tsize.x() * 0.5);
      int bidx = -1;
      float min_diff = std::numeric_limits<float>::max();
      float try_ = -1;

      int start = raw_sel - 1;
      if (tpos.y() == layouts[0][0].pos.y()) {
        start = (int)layouts[0].size();
      }

      for (int i = start; i >= 0; i--) {
        auto& tk = layouts[0][i];

        if (tk.pos.y() != tpos.y()) {
          if (try_ == -1) {
            try_ = tk.pos.y();
          }
          if (tk.pos.y() != try_) {
            break;
          }

          float tcenl = tk.pos.x() + (tk.size.x() * 0.5);
          float diff = std::abs(tcen - tcenl);
          if (diff < min_diff) {
            min_diff = diff;
            bidx = i;
          }
        }
      }

      if (bidx != -1) {
        raw_sel = bidx;
      }
      MoveSelector();
    }

    /// Go Down Movement
    if (inp->IsUp(inp->Down)) {
      vec2 tpos = layouts[0][raw_sel].pos;
      vec2 tsize = layouts[0][raw_sel].size;
      float tcen = tpos.x() + (tsize.x() * 0.5);
      int bidx = -1;
      float min_diff = std::numeric_limits<float>::max();
      float try_ = -1;

      int start = raw_sel + 1;
      if (tpos.y() == layouts[0][layouts[0].size() - 1].pos.y()) {
        start = 0;
      }

      for (int i = start; i < (int)layouts[0].size(); i++) {
        auto& tk = layouts[0][i];

        if (tk.pos.y() != tpos.y()) {
          if (try_ == -1) {
            try_ = tk.pos.y();
          }
          if (tk.pos.y() != try_) {
            break;
          }

          float tcenl = tk.pos.x() + (tk.size.x() * 0.5);
          float diff = std::abs(tcen - tcenl);
          if (diff < min_diff) {
            min_diff = diff;
            bidx = i;
          }
        }
      }

      if (bidx != -1) {
        raw_sel = bidx;
      }
      MoveSelector();
    }
    /// Go right movement
    if (inp->IsUp(inp->Right)) {
      if ((raw_sel + 1 >= (int)layouts[0].size()) ||
          layouts[0][raw_sel].pos.y() != layouts[0][raw_sel + 1].pos.y()) {
        for (int i = raw_sel - 1; i > 0; i--) {
          if (i - 1 <= 0) {
            raw_sel = 0;
            break;
          }
          if (layouts[0][i].pos.y() != layouts[0][i - 1].pos.y()) {
            raw_sel = i;
            break;
          }
        }
      } else {
        raw_sel++;
      }
      MoveSelector();
    }
    // Go left Movement
    if (inp->IsUp(inp->Left)) {
      if (raw_sel - 1 < 0 ||
          layouts[0][raw_sel].pos.y() != layouts[0][raw_sel - 1].pos.y()) {
        for (int i = raw_sel; i < (int)layouts[0].size(); i++) {
          if (i >= (int)layouts[0].size()) {
            raw_sel = (int)layouts[0].size();
          }
          if (layouts[0][i].pos.y() != layouts[0][i + 1].pos.y()) {
            raw_sel = i;
            break;
          }
        }
      } else {
        raw_sel--;
      }
      MoveSelector();
    }
  }
}

void Keyboard::DoOperation(KeyOperation op, const std::string& kname) {
  switch (op) {
    case AppendSelf:
      *text += kname;
      if (mode == 2) {
        mode = 0;
      }
      break;
    case OpCancel:
      Rem();
      *text = copy;
      break;
    case OpConfirm:
      Rem();
      break;
    case Shift:
      mode = mode == 2 ? 0 : 2;
      break;
    case Caps:
      mode = mode == 1 ? 0 : 1;
      break;
    case Backspace: {
      std::string c = *text;
      *text = c.substr(0, c.size() - 1);
    } break;
    case Space:
      *text += ' ';
      break;
    case Tab:
      *text += '\t';
      break;

    default:
      break;
  }
}

void Keyboard::RecolorBy(KeyOperation op, u32 color, int cm) {
  int i = 0;
  /// Not the fastest but the best for custom layouts
  for (auto& it : layouts[cm]) {
    if (it.op == op) {
      keys[cm]->ReColorQuad(i, PD::Color(0xaa222222));
      break;
    }
    i++;
  }
}
void Keyboard::InputOpBind(Hid::Key k, KeyOperation op, Hid::Ref inp, int cm) {
  if (inp->IsUp(k)) {
    RecolorBy(op, PD::Color(0xaa222222), cm);
    DoOperation(op, "");
  } else if (inp->IsHeld(k)) {
    RecolorBy(op, PD::Color(0xaa333333), cm);
  }
}

void Keyboard::Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) {
  /// Load Keys if not present
  if (!keys_loadet) {
    inp->Clear();
    LoadTheKeys(ren);
  }
  /// Unlock Input
  inp->Unlock();
  /// Kill Overlay if rem was toggeled and
  /// Animation is finished
  if (rem && flymgr.IsFinished()) {
    // Should be already unlocked ...
    // ist mit aber egal
    inp->Unlock();
    Kill();
    return;  // Break to not lock again
  }
  /// Process Controller Movement
  Movement(inp);

  /// Update animations
  flymgr.Update(delta);
  selector.Update(delta);
  sel_szs.Update(delta);
  /// Blend Top or|and Bottom Screen
  if (flags & Flags_BlendBottom || flags & Flags_BlendTop) {
    Color fade(0.3f, 0.3f, 0.3f);
    if (rem) {
      fade.a(fade.a() * (1.f - flymgr.Progress()));
    } else {
      fade.a(fade.a() * flymgr.Progress());
    }
    if (flags & Flags_BlendTop) {
      ren->OnScreen(ren->GetScreen(false));
      ren->DrawRectSolid(0, fvec2(400, 240), fade);
    }
    if (flags & Flags_BlendBottom) {
      ren->OnScreen(ren->GetScreen(true));
      ren->DrawRectSolid(0, fvec2(320, 240), fade);
    }
  }
  /// Get the current start possition
  vec2 start = flymgr;
  // Draw head and Keyboard background
  ren->DrawRectSolid(
      fvec2(0, start.y()), fvec2(320, 125),
      PD::Color("#222222ff").a((flags & Flags_Transparency) ? 0xaa : 0xff));
  ren->DrawRectSolid(fvec2(0, start.y()), fvec2(320, 17), 0xaa000000);
  /// Grab the base layer and go one up for texts
  int l = ren->Layer();
  ren->Layer(l + 2);
  // if (ren->Font()->SystemFont()) {
  //   std::stringstream s;
  //   s << GamePadIcons::GetIcon(GamePadIcons::B) << " Backspace ";
  //   s << GamePadIcons::GetIcon(GamePadIcons::Y) << " Space\n";
  //   s << GamePadIcons::GetIcon(GamePadIcons::X) << " Cancel ";
  //   s << GamePadIcons::GetIcon(GamePadIcons::Start) << " Confirm\n";
  //   s << GamePadIcons::GetIcon(GamePadIcons::L) << " Shift ";
  //   s << GamePadIcons::GetIcon(GamePadIcons::R) << " CAPS\n";
  //   s << GamePadIcons::GetIcon(GamePadIcons::Dpad) << " Move ";
  //   s << GamePadIcons::GetIcon(GamePadIcons::A) << " Select\n";
  //   ren->DrawText(fvec2(5, start.y() -
  //   ren->GetTextDimensions(s.str()).y()+16),
  //                 0xffffffff, s.str());
  // }
  ren->DrawText(fvec2(5, start.y()), 0xffffffff, "> " + *text);
  ren->Layer(l + 1);
  /// Offset Keys start height by 22
  start[1] += 22;
  /// Cache Mode to not render on 0, 0
  int cm = mode;
  keys[cm]->ReCopy();
  int ii = 0;
  for (auto& it : layouts[mode]) {
    PD::Color bgc(0xaa444444);
    if (((ren->InBox(inp->TouchPosLast(), vec4(start + it.pos, it.size)) &&
          inp->IsHeld(inp->Touch)) ||
         (inp->IsHeld(inp->A) && ii == raw_sel)) &&
        flymgr.IsFinished()) {
      bgc = PD::Color(0xaa333333);
    }
    if (((ren->InBox(inp->TouchPosLast(), vec4(start + it.pos, it.size)) &&
          inp->IsUp(inp->Touch)) ||
         (inp->IsUp(inp->A) && ii == raw_sel)) &&
        flymgr.IsFinished()) {
      bgc = PD::Color(0xaa222222);
      DoOperation(it.op, it.k);
    }
    /// This is hardcoded shit guessing that the
    /// Buttons are in the beginning of the list
    /// (Should be the case as OptiCmdList sorts by layer)
    keys[cm]->ReColorQuad(ii, bgc);
    ii++;
  }

  // Bind Key Operations
  InputOpBind(inp->B, Backspace, inp, cm);
  InputOpBind(inp->Y, Space, inp, cm);
  InputOpBind(inp->Start, OpConfirm, inp, cm);
  InputOpBind(inp->X, OpCancel, inp, cm);
  InputOpBind(inp->L, Shift, inp, cm);
  InputOpBind(inp->R, Caps, inp, cm);

  if (raw_sel != -1) {
    ren->Layer(l);
    ren->DrawRectSolid(start + selector - fvec2(1), fvec2(sel_szs) + fvec2(2),
                       0xaaffffff);
    ren->Layer(l);
  }
  keys[cm]->ReLayer(l);
  keys[cm]->MoveIt(start);
  for (auto it : keys[cm]->List()) {
    ren->PushCommand(it);
  }
  inp->Lock();
}
}  // namespace PD