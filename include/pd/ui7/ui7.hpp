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
#include <pd/ui7/flags.hpp>
#include <pd/ui7/id.hpp>
#include <pd/ui7/menu.hpp>
#include <pd/ui7/theme.hpp>
#include <unordered_map>

namespace PD {
namespace UI7 {
class Context : public SmartCtor<Context> {
 public:
  Context() {}
  ~Context() {}

  void Update(float delta);

 private:
  // Timing
  float delta;
  float time;
  float last;
  // Context
  bool in_menu;
  // Debug
  bool debugging;
  // Menu Handlers
  std::unordered_map<u32, Menu::Ref> menus;
  Menu::Ref current;
  // Context DrawList
  DrawList::Ref debug;
  DrawList::Ref front;
  DrawList::Ref back;
  // Promt Handler
};
}  // namespace UI7
}  // namespace PD