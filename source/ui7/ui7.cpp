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

#include <pd/core/timetrace.hpp>
#include <pd/ui7/ui7.hpp>

// Helpers

#define UI7DV4(x)                                                           \
  std::format("{}: [{:.2f}, {:.2f}, {:.2f}, {:.2f}]", #x, x[0], x[1], x[2], \
              x[3])
#define UI7DV4N(x) \
  std::format("[{:.2f}, {:.2f}, {:.2f}, {:.2f}]", x[0], x[1], x[2], x[3])
#define UI7DV2(x) std::format("{}: [{:.2f}, {:.2f}]", #x, x[0], x[1])
#define UI7DV2N(x) std::format("[{:.2f}, {:.2f}]", x[0], x[1])
#define UI7DHX32(x) std::format("{}: {:#08x}", #x, x)
#define UI7DTF(x) PD::Strings::FormatNanos(x)

namespace PD {
std::string UI7::GetVersion(bool show_build) {
  std::stringstream s;
  s << ((UI7_VERSION >> 24) & 0xFF) << ".";
  s << ((UI7_VERSION >> 16) & 0xFF) << ".";
  s << ((UI7_VERSION >> 8) & 0xFF);
  if (show_build) s << "-" << ((UI7_VERSION) & 0xFF);
  return s.str();
}
bool UI7::Context::BeginMenu(const ID& id, UI7MenuFlags flags) {
  Assert(!this->current, "You are already in another Menu!");
  Assert(std::find(amenus.begin(), amenus.end(), (u32)id) == amenus.end(),
         "Menu Name Already used or\nContext::Update not called!");
  auto menu = this->menus.find(id);
  if (menu == this->menus.end()) {
    this->menus[id] = Menu::New(id, io);
    this->menus[id]->ViewArea(this->io->Ren->GetViewport());
    menu = this->menus.find(id);
  }
  this->current = menu->second;
  if (!this->current->BackList()) {
    this->current->BackList(DrawList::New(io->Ren));
    io->RegisterDrawList(this->current->name + "back", this->current->back);
  }
  if (!this->current->MainList()) {
    this->current->MainList(DrawList::New(io->Ren));
    io->RegisterDrawList(this->current->name + "main", this->current->main);
  }
  if (!this->current->FrontList()) {
    this->current->FrontList(DrawList::New(io->Ren));
    io->RegisterDrawList(this->current->name + "front", this->current->front);
  }
  this->current->PreHandler(flags);
  amenus.push_back(this->current->GetID());
  if (!this->current->is_open) {
    this->current = nullptr;
  }
  return this->current != nullptr;
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
  this->io->Delta = delta;
  io->DeltaStats->Add(io->Delta * 1000);
  for (auto it : amenus) {
    menus[it]->Update(io->Delta);
  }
  int list = 1;
  for (auto it : io->DrawListRegestry) {
    it.second->BaseLayer(list * 10);
    it.second->Process();
    list++;
  }
  this->amenus.clear();
  this->io->Update();
}

void UI7::Context::AboutMenu() {
  if (this->BeginMenu("About UI7", UI7MenuFlags_Scrolling)) {
    auto m = this->GetCurrentMenu();

    m->Label("Palladium - UI7 " + GetVersion());
    m->Separator();
    m->Label("(c) 2023-2025 René Amthor");
    m->Label("UI7 is licensed under the MIT License.");
    m->Label("See LICENSE for more information.");
    static bool show_build;
    m->Checkbox("Show Build Info", show_build);
    if (show_build) {
      m->SeparatorText("Build Info");
      m->Label("Full Version -> " + GetVersion(true));
      m->Label("sizeof(size_t) -> " + std::to_string(sizeof(size_t)));
      m->Label("sizeof(LI::Vertex) -> " + std::to_string(sizeof(LI::Vertex)));
      m->Label("__cplusplus -> " + std::to_string(__cplusplus));
      m->Label("Compiler -> " + LibInfo::CompiledWith());
    }
    this->EndMenu();
  }
}

void UI7::Context::MetricsMenu() {
  if (this->BeginMenu("UI7 Metrics", UI7MenuFlags_Scrolling)) {
    auto m = this->GetCurrentMenu();

    m->Label("Palladium - UI7 " + GetVersion());
    m->Separator();
    m->Label(
        std::format("Average {:.3f} ms/f ({:.1f} FPS)",
                    ((float)io->DeltaStats->GetAverage() / 1000.f),
                    1000.f / ((float)io->DeltaStats->GetAverage() / 1000.f)));
    m->Label("Menus: " + std::to_string(menus.size()));
    m->SeparatorText("Lithium");
    m->Label(std::format("Vertices: {} Indices: {}", io->Ren->Vertices(),
                         io->Ren->Indices()));
    m->Label("Triangles: " + std::to_string(io->Ren->Indices() / 3));
    m->SeparatorText("TimeTrace");
    if (m->BeginTreeNode("Traces (" +
                         std::to_string(Sys::GetTraceMap().size()) + ")")) {
      for (auto& it : Sys::GetTraceMap()) {
        if (m->BeginTreeNode(it.second->GetID())) {
          m->Label("Diff: " + UI7DTF(it.second->GetLastDiff()));
          m->Label("Protocol Len: " +
                   std::to_string(it.second->GetProtocol()->GetLen()));
          m->Label("Average: " +
                   UI7DTF(it.second->GetProtocol()->GetAverage()));
          m->Label("Min: " + UI7DTF(it.second->GetProtocol()->GetMin()));
          m->Label("Max: " + UI7DTF(it.second->GetProtocol()->GetMax()));
          m->EndTreeNode();
        }
      }
      m->EndTreeNode();
    }
    m->SeparatorText("IO");
    if (m->BeginTreeNode("Menus (" + std::to_string(menus.size()) + ")")) {
      for (auto& it : menus) {
        if (m->BeginTreeNode(it.second->name)) {
          m->Label("Name: " + it.second->name);
          m->Label("Pos: " + UI7DV2N(it.second->view_area.xy()));
          m->Label("Size: " + UI7DV2N(it.second->view_area.zw()));
          m->Label("Main Area: " + UI7DV4N(it.second->main_area));
          m->Label("Cursor: " + UI7DV2N(it.second->cursor));
          if (m->BeginTreeNode("ID Objects (" +
                               std::to_string(it.second->idobjs.size()) +
                               ")")) {
            for (auto& jt : it.second->idobjs) {
              m->Label(UI7DHX32(jt->GetID()));
            }
            m->EndTreeNode();
          }
          m->EndTreeNode();
        }
      }
      m->EndTreeNode();
    }
    if (m->BeginTreeNode("DrawLists (" +
                         std::to_string(io->DrawListRegestry.size()) + ")")) {
      for (auto& it : io->DrawListRegestry) {
        m->Label(it.first.GetName());
      }
      m->EndTreeNode();
    }
    m->Label("io->Time: " + Strings::FormatMillis(io->Time->Get()));
    m->Label(std::format("io->Delta: {:.3f}", io->Delta));
    m->Label(std::format("io->Framerate: {:.2f}", io->Framerate));
    m->Label(UI7DHX32(io->DraggedObject));
    m->Label(std::format("io->DragTime: {:.2f}s", io->DragTime->GetSeconds()));
    m->Label(UI7DV4(io->DragDestination));
    m->Label(UI7DV2(io->DragSourcePos));
    m->Label(UI7DV2(io->DragPosition));
    m->Label(UI7DV2(io->DragLastPosition));
    this->EndMenu();
  }
}

void UI7::Context::StyleEditor() {
  if (this->BeginMenu("UI7 Style Editor", UI7MenuFlags_Scrolling)) {
    auto m = this->GetCurrentMenu();

    m->Label("Palladium - UI7 " + GetVersion() + " Style Editor");
    m->Separator();
    m->Label(std::format("MenuPadding: {}, {}", io->MenuPadding.x(),
                         io->MenuPadding.y()));
    m->SameLine();
    if (m->Button("-")) {
      io->MenuPadding -= 1;
    }
    m->SameLine();
    if (m->Button("+")) {
      io->MenuPadding += 1;
    }
    m->Label(std::format("FramePadding: {}, {}", io->FramePadding.x(),
                         io->FramePadding.y()));
    m->SameLine();
    if (m->Button("-")) {
      io->FramePadding -= 1;
    }
    m->SameLine();
    if (m->Button("+")) {
      io->FramePadding += 1;
    }
    m->Label(
        std::format("ItemSpace: {}, {}", io->ItemSpace.x(), io->ItemSpace.y()));
    m->SameLine();
    if (m->Button("-")) {
      io->ItemSpace -= 1;
    }
    m->SameLine();
    if (m->Button("+")) {
      io->ItemSpace += 1;
    }
    this->EndMenu();
  }
}
}  // namespace PD