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
  }
  if (!this->current->MainList()) {
    this->current->MainList(DrawList::New(ren));
  }
  if (!this->current->FrontList()) {
    this->current->FrontList(DrawList::New(ren));
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

void UI7::Context::EndMenu() {
  this->current->PostHandler();
  this->current = nullptr;
}

void UI7::Context::Update(float delta) {
  TT::Scope st("UI7_Update");
  Assert(current == nullptr, "Still in a Menu!");
  this->back->BaseLayer(10);
  this->back->Process();
  for (auto it : amenus) {
    menus[it]->Update(delta);
  }
  this->front->BaseLayer(60);
  this->front->Process();
  this->amenus.clear();
}
}  // namespace PD