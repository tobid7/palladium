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
bool UI7::Context::BeginMenu(const ID& id, UI7MenuFlags flags, bool* show) {
  Assert(!this->current, "You are already in another Menu!");
  Assert(std::find(amenus.begin(), amenus.end(), (u32)id) == amenus.end(),
         "Menu Name Already used or\nContext::Update not called!");
  if (show != nullptr) {
    if (!(*show)) {
      if (io->FocusedMenu == id) {
        io->FocusedMenu = 0;
        io->FocusedMenuRect = 0;
      }
      return false;
    }
  }
  auto menu = this->menus.find(id);
  if (menu == this->menus.end()) {
    this->menus[id] = Menu::New(id, io);
    this->menus[id]->Layout->SetSize(io->Ren->GetViewport().zw());
    menu = this->menus.find(id);
  }
  this->current = menu->second;
  this->current->is_shown = show;
  this->io->CurrentMenu = this->current->id;
  io->RegisterDrawList(id, this->current->Layout->GetDrawList());
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
  this->io->CurrentMenu = 0;
}

void UI7::Context::Update(float) {
  TT::Scope st("UI7_Update");
  Assert(current == nullptr, "Still in a Menu!");
  bool focused_exist = false;
  for (auto it : amenus) {
    auto m = menus[it];
    io->CurrentMenu = m->id;
    m->Update(io->Delta);
    io->CurrentMenu = 0;
    if (it == io->FocusedMenu) {
      focused_exist = true;
    }
  }
  if (!focused_exist) {
    io->FocusedMenu = 0;
    io->FocusedMenuRect = 0;
  }
  int list = 0;
  u32 vtx_counter = 0;
  u32 idx_counter = 0;
  // Render the Focused Menu Last
  std::sort(io->DrawListRegestry.begin(), io->DrawListRegestry.end(),
            [&](const auto& a, const auto& b) {
              return (a.first == io->FocusedMenu) <
                     (b.first == io->FocusedMenu);
            });
  // Register Front List as last element
  io->RegisterDrawList("CtxFrontList", io->Front);
  for (auto it : io->DrawListRegestry) {
    it.second->BaseLayer(list * 30);
    it.second->Process();
    vtx_counter += it.second->num_vertices;
    idx_counter += it.second->num_indices;
    list++;
  }
  io->NumIndices = idx_counter;
  io->NumVertices = vtx_counter;
  this->amenus.clear();
  this->io->Update();
}

void UI7::Context::AboutMenu(bool* show) {
  if (this->BeginMenu("About UI7", UI7MenuFlags_Scrolling, show)) {
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

void UI7::Context::MetricsMenu(bool* show) {
  if (this->BeginMenu("UI7 Metrics", UI7MenuFlags_Scrolling, show)) {
    auto m = this->GetCurrentMenu();

    m->Label("Palladium - UI7 " + GetVersion());
    m->Separator();
    m->Label(
        std::format("Average {:.3f} ms/f ({:.1f} FPS)",
                    ((float)io->DeltaStats->GetAverage() / 1000.f),
                    1000.f / ((float)io->DeltaStats->GetAverage() / 1000.f)));
    m->Label(std::format("NumVertices: {}", io->NumVertices));
    m->Label(std::format("NumIndices: {} -> {} Tris", io->NumIndices,
                         io->NumIndices / 3));
    m->Label("Menus: " + std::to_string(menus.size()));
    if (m->BeginTreeNode("Font")) {
      for (u32 i = 0; i <= 0x00ff; i++) {
        auto& c = io->Ren->Font()->GetCodepoint(i);
        if (!c.invalid()) {
          m->Image(c.tex(), c.size(), c.uv());
          if ((i % 15) != 0 || i == 0) {
            m->SameLine();
          }
        }
      }
      m->EndTreeNode();
    }
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
          m->Label("Pos: " + UI7DV2N(it.second->Layout->GetPosition()));
          m->Label("Size: " + UI7DV2N(it.second->Layout->GetSize()));
          m->Label("Work Rect: " + UI7DV4N(it.second->Layout->WorkRect));
          m->Label("Cursor: " + UI7DV2N(it.second->Layout->Cursor));
          if (m->BeginTreeNode(
                  "ID Objects (" +
                  std::to_string(it.second->Layout->IDObjects.size()) + ")")) {
            for (auto& jt : it.second->Layout->IDObjects) {
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
        if (m->BeginTreeNode(it.first.GetName())) {
          m->Label("Vertices: " + std::to_string(it.second->num_vertices));
          m->Label("Indices: " + std::to_string(it.second->num_indices));
          m->Label("Base Layer: " + std::to_string(it.second->base));
          m->EndTreeNode();
        }
      }
      m->EndTreeNode();
    }
    m->Label("io->Time: " + Strings::FormatMillis(io->Time->Get()));
    m->Label(std::format("io->Delta: {:.3f}", io->Delta));
    m->Label(std::format("io->Framerate: {:.2f}", io->Framerate));
    m->Label(UI7DHX32(io->FocusedMenu));
    m->Label(UI7DHX32(io->DraggedObject));
    m->Label(std::format("io->DragTime: {:.2f}s", io->DragTime->GetSeconds()));
    m->Label(UI7DV4(io->DragDestination));
    m->Label(UI7DV2(io->DragSourcePos));
    m->Label(UI7DV2(io->DragPosition));
    m->Label(UI7DV2(io->DragLastPosition));
    this->EndMenu();
  }
}

void UI7::Context::StyleEditor(bool* show) {
  if (this->BeginMenu("UI7 Style Editor", UI7MenuFlags_Scrolling, show)) {
    auto m = this->GetCurrentMenu();

    m->Label("Palladium - UI7 " + GetVersion() + " Style Editor");
    m->Separator();
    m->DragData("MenuPadding", (float*)&io->MenuPadding, 2, 0.f, 100.f);
    m->DragData("FramePadding", (float*)&io->FramePadding, 2, 0.f, 100.f);
    m->DragData("ItemSpace", (float*)&io->ItemSpace, 2, 0.f, 100.f);
    m->DragData("MinSliderSize", (float*)&io->MinSliderDragSize, 2, 1.f, 100.f);
    m->DragData("OverScroll Modifier", &io->OverScrollMod, 1, 0.01f,
                std::numeric_limits<float>::max(), 0.01f, 2);
    m->SeparatorText("Theme");
    if (m->Button("Dark")) {
      UI7::Theme::Default(*io->Theme.get());
    }
    m->SameLine();
    if (m->Button("Flashbang")) {
      UI7::Theme::Flashbang(*io->Theme.get());
    }
    /// Small trick to print without prefix
#define ts(x) m->ColorEdit(std::string(#x).substr(9), &io->Theme->GetRef(x));
#define ts2(x)                                                                 \
  m->DragData(std::string(#x).substr(9), (u8*)&io->Theme->GetRef(x), 4, (u8)0, \
              (u8)255);
    ts2(UI7Color_Background);
    ts2(UI7Color_Button);
    ts2(UI7Color_ButtonDead);
    ts2(UI7Color_ButtonActive);
    ts2(UI7Color_ButtonHovered);
    ts2(UI7Color_Text);
    ts2(UI7Color_TextDead);
    ts2(UI7Color_Header);
    ts2(UI7Color_HeaderDead);
    ts2(UI7Color_Selector);
    ts2(UI7Color_Checkmark);
    ts2(UI7Color_FrameBackground);
    ts2(UI7Color_FrameBackgroundHovered);
    ts2(UI7Color_Progressbar);
    ts2(UI7Color_ListEven);
    ts2(UI7Color_ListOdd);
    this->EndMenu();
  }
}
}  // namespace PD