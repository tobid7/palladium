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

#include <pd/common/timetrace.hpp>
#include <pd/ui7/ui7.hpp>

namespace PD {
bool UI7::Context::BeginMenu(const ID& id, UI7MenuFlags flags) {
  Assert(!this->current, "You are already in another Menu!");
  Assert(std::find(amenus.begin(), amenus.end(), (u32)id) == amenus.end(),
         "Menu Name Already used or\nContext::Update not called!");
  auto menu = this->menus.find(id);
  if (menu == this->menus.end()) {
    this->menus[id] = Menu::New(id, &theme, inp);
    menu = this->menus.find(id);
  }
  this->current = menu->second;
  if (!this->current->BackList()) {
    this->current->BackList(DrawList::New(ren));
    this->current->BackList()->BaseLayer(root_layer + 30);
  }
  if (!this->current->MainList()) {
    this->current->MainList(DrawList::New(ren));
    this->current->MainList()->BaseLayer(root_layer + 40);
  }
  if (!this->current->FrontList()) {
    this->current->FrontList(DrawList::New(ren));
    this->current->FrontList()->BaseLayer(root_layer + 50);
  }
  this->current->ViewArea(this->ren->GetViewport());
  this->current->PreHandler(flags);
  amenus.push_back(this->current->GetID());
  return true;
}

UI7::Menu::Ref UI7::Context::GetCurrentMenu() {
  Assert(current != nullptr, "Not in a Menu!");
  return current;
}

UI7::Menu::Ref UI7::Context::FindMenu(const ID& id) {
  auto e = this->menus.find(id);
  if (e != this->menus.end()) {
    return e->second;
  }
  return nullptr;
}

void UI7::Context::EndMenu() {
  this->current->PostHandler();
  this->current = nullptr;
}

void UI7::Context::Update(float delta) {
  TT::Scope st("UI7_Update");
  Assert(current == nullptr, "Still in a Menu!");
  this->back->BaseLayer(root_layer + 10);
  this->back->Process();
  for (auto it : amenus) {
    menus[it]->Update(delta);
  }
  this->front->BaseLayer(root_layer + 60);
  this->front->Process();
  this->amenus.clear();
}
}  // namespace PD