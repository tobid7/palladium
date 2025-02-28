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

#include <pd/core/common.hpp>
#include <pd/core/strings.hpp>
#include <pd/drivers/hid.hpp>
#include <pd/core/vec.hpp>
#include <pd/ui7/drawlist.hpp>

namespace PD {
namespace UI7 {
class Container : public SmartCtor<Container> {
 public:
  Container() {}
  Container(const vec2& pos, const vec2& size) : pos(pos), size(size) {}
  Container(const vec4& box) : pos(box.xy()), size(box.zw()) {}
  ~Container() {}

  void Init(LI::Renderer::Ref r, UI7::DrawList::Ref l, UI7::Theme::Ref lt) {
    list = l;
    theme = lt;
    ren = r;
  }

  void SetPos(const vec2& pos) { this->pos = pos; }
  void SetSize(const vec2& size) { this->size = size; }

  vec2 GetPos() { return pos; }
  vec2 GetSize() { return size; }

  void SetParent(Container::Ref v) { parent = v; }
  Container::Ref GetParent() { return parent; }

  bool Skippable() const { return skippable; }
  bool Removable() const { return rem; }

  void HandleScrolling(vec2 scrolling, vec4 viewport);
  virtual void HandleInput(Hid::Ref inp) {}
  virtual void Draw() {}

  void UnlockInput() { inp_done = false; }

  u32 GetID() const { return id; }
  void SetID(u32 id) { this->id = id; }

 protected:
  /// used to skip Input/Render preocessing ot not
  bool skippable = false;
  bool rem = false;
  u64 last_use = 0;
  bool inp_done = false;
  Screen::Ref screen;
  vec2 pos;
  vec2 size;
  UI7::DrawList::Ref list;
  UI7::Theme::Ref theme;
  LI::Renderer::Ref ren;
  Container::Ref parent;
  u32 id = 0;
};
}  // namespace UI7
}  // namespace PD
