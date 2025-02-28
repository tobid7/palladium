#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd/core/common.hpp>
#include <pd/lithium/command.hpp>
#include <pd/lithium/flags.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/lithium/vertex.hpp>
#include <pd/core/vec.hpp>

namespace PD {
namespace LI {
class Renderer;
class StaticObject : public SmartCtor<StaticObject> {
 public:
  StaticObject() {}
  ~StaticObject() {}

  void PushCommand(Command::Ref v) { cmds.push_back(v); }

  void ReCopy() {
    cpy.clear();
    for (auto it : cmds) {
      cpy.push_back(Command::New(it));
    }
  }

  void ReColorQuad(int idx, u32 col) {
    if (idx > (int)cpy.size()) {
      return;
    }
    for (auto& it : cpy[idx]->VertexList()) {
      it.Color(col);
    }
  }

  void MoveIt(vec2 off) {
    for (auto& it : cpy) {
      for (auto& jt : it->VertexList()) {
        jt.pos += off;
      }
    }
  }

  void ReColor(u32 col) {
    for (auto& it : cpy) {
      for (auto& jt : it->VertexList()) {
        jt.Color(col);
      }
    }
  }

  void ReLayer(int base_layer) {
    for (auto& it : cpy) {
      it->Layer(it->Layer() + base_layer);
    }
  }

  void ReIndex(int start) {
    for (int i = 0; i < (int)cpy.size(); i++) {
      cpy[i]->Index(start + i);
    }
  }

  std::vector<Command::Ref>& List() {
    if (cpy.size() != 0) {
      return cpy;
    }
    return cmds;
  }

 private:
  std::vector<Command::Ref> cpy;
  std::vector<Command::Ref> cmds;
};

class TextBox {
 public:
  TextBox() {}
  TextBox(const vec2& s, float time) {
    size = s;
    time_created = time;
    optional = false;
  }
  ~TextBox() {}

  void TimeCreated(float v) { time_created = v; }
  void Size(const vec2& v) { size = v; }
  void Optional(bool v) { optional = v; }
  void Text(const std::string& v) { text = v; }

  vec2 Size() const { return size; }
  float TimeCreated() const { return time_created; }
  bool Optional() const { return optional; }
  std::string Text() const { return text; }

 private:
  vec2 size;
  float time_created;
  bool optional;
  std::string text;  // TextWrap
};

class StaticText : public SmartCtor<StaticText> {
 public:
  StaticText() {}
  StaticText(Renderer* ren, const vec2& pos, u32 clr, const std::string& text,
             LITextFlags flags = 0, const vec2& box = 0) {
    Setup(ren, pos, clr, text, flags, box);
  }
  ~StaticText() {}

  void Setup(Renderer* ren, const vec2& pos, u32 clr, const std::string& text,
             LITextFlags flags = 0, const vec2& box = 0);

  vec2 GetDim() const { return tdim; }
  vec2 GetPos() const { return pos; }

  void SetColor(u32 col);
  void SetPos(const vec2& pos);
  void SetLayer(int l);

  void SetUnused() { used = false; }
  bool Used() const { return used; }

  bool IsSetup() { return text != nullptr; }

  void Draw();

  void Font(Font::Ref fnt) { font = fnt; }
  Font::Ref Font() { return font; }

 private:
  Font::Ref font;
  bool used;
  Renderer* ren;
  vec2 tdim;
  vec2 pos;
  StaticObject::Ref text;
};
}  // namespace LI
}  // namespace PD