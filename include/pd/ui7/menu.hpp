#pragma once

/*
MIT License
Copyright (c) 2024 René Amthor (tobid7)

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

#include <pd/ui7/drawlist.hpp>

namespace PD {
namespace UI7 {
class Menu : public SmartCtor<Menu> {
 public:
  Menu(u32 id) {
    this->id = id;
    scrolling[0] = false;
    scrolling[1] = false;
    scrollbar[0] = false;
    scrollbar[1] = false;
    scroll_allowed[0] = false;
    scroll_allowed[1] = false;
  };
  ~Menu() {};

 private:
  u32 id;
  vec2 cursor;
  vec2 bcursor;
  vec2 slcursor;
  vec4 view_area;
  vec2 scrolling_off;
  bool scrolling[2];
  vec2 scroll_mod;
  float tbh;
  bool scrollbar[2];
  bool scroll_allowed[2];
  bool has_touch;

  Menu::Ref submenu;

  // DrawLists
  DrawList::Ref back;
  DrawList::Ref main;
  DrawList::Ref front;

  vec2 max;
  vec2 mouse;
  vec2 bslpos;
  vec2 last_size;
};
}  // namespace UI7
}  // namespace PD