/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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

#include <pd/ui7/ui7.hpp>

#include "pd/pd_p_api.hpp"
#include "pd/ui7/flags.hpp"

#define UI7DHX32(x) std::format("{}: {:#08x}", #x, x)
#define UI7DTF(x) PD::Strings::FormatNanos(x)

namespace PD {
namespace UI7 {
PD_API std::string GetVersion(bool show_build) {
  std::stringstream s;
  s << ((UI7_VERSION >> 24) & 0xFF) << ".";
  s << ((UI7_VERSION >> 16) & 0xFF) << ".";
  s << ((UI7_VERSION >> 8) & 0xFF);
  if (show_build) s << "-" << ((UI7_VERSION) & 0xFF);
  return s.str();
}

PD_API void Context::AddViewPort(const ID& id, const ivec4& vp) {
  pIO.AddViewPort(id, vp);
}

PD_API void Context::UseViewPort(const ID& id) {
  if (!pIO.ViewPorts.count(id)) {
    return;
  }
  pIO.CurrentViewPort = pIO.ViewPorts[id.RawID()];
}

PD_API Menu* Context::BeginMenu(const ID& id, UI7MenuFlags flags, bool* pShow) {
  if (pCurrent) {
    PDERR("UI7: You are already in {} Menu!", pCurrent->pID.GetName());
    return nullptr;
  }
  if (std::find(pCurrentMenus.begin(), pCurrentMenus.end(), (u32)id) !=
      pCurrentMenus.end()) {
    PDERR("UI7: Menu {} already exists!", id.GetName());
    return nullptr;
  }
  pCurrent = pGetOrCreateMenu(id);
  this->pCurrent->pIsShown = pShow;
  if (pCurrent->pIsShown != nullptr) {
    if (!*pCurrent->pIsShown) {
      pCurrent = nullptr;
      return nullptr;
    }
  }
  /** Probably we dont even need Input Handling in this stage */
  // this->pIO.InputHandler.CurrentMenu = id;
  pCurrentMenus.push_back(id);
  pCurrent->Flags = flags;
  if (!pCurrent->pIsOpen) {
    pCurrent = nullptr;
  }
  return pCurrent;
}

PD_API void Context::EndMenu() {
  /**
   * Currently it would be a better wy to handle menus as follows
   *
   * The folowing context will generate a new menu as normally but instead
   * of true or false we have m is false (nullptr) or true (some ptr returned)
   * and after that it should simply out of scope
   * (This would probably require some wrapper class to find out if m goes
   * out of scope)
   * ```cpp
   *  if(auto m = ui7->BeginMenu("Test")) {
   *    m->Label("Show some Text");
   *  }
   * ```
   */
  if (!pCurrent) {
    return;
  }
  pCurrent = nullptr;
  // pIO.InputHandler.CurrentMenu = 0;
}

PD_API void Context::Update() {
  /**
   * Cause Commenting each line looks carbage...
   * This function simply clears the FinalDrawList, Searches for Menu ID's in
   * The sorted menu List from last frame to insert them as same order into
   * the final list. After that it adds new menus to the begin to 'add' new
   * menus on top. As final step the focused menu gets add to begin
   * Then the menus update their Input and DraeList Generation in List Order
   * and the DrawLists get Merged into the FDL in reverse Order. At end the List
   * gets cleanup and io gets updated
   *
   * Very simple ...
   */
  pIO.FDL.Clear();
  if (std::find(pCurrentMenus.begin(), pCurrentMenus.end(),
                pIO.InputHandler.FocusedMenu) == pCurrentMenus.end()) {
    pIO.InputHandler.FocusedMenu = 0;
    pIO.InputHandler.FocusedMenuRect = fvec4(0);
  }
  std::vector<u32> FinalList;
  for (auto it : pDFO) {
    if (std::find(pCurrentMenus.begin(), pCurrentMenus.end(), it) !=
            pCurrentMenus.end() &&
        it != pIO.InputHandler.FocusedMenu) {
      FinalList.push_back(it);
    }
  }
  for (auto it : pCurrentMenus) {
    if (std::find(FinalList.begin(), FinalList.end(), it) == FinalList.end() &&
        it != pIO.InputHandler.FocusedMenu) {
      FinalList.push_back(it);
    }
  }
  if (pMenus.count(pIO.InputHandler.FocusedMenu)) {
    FinalList.insert(FinalList.begin(), pIO.InputHandler.FocusedMenu);
  }
  pDFO = FinalList;
  for (auto& it : FinalList) {
    this->pIO.InputHandler.CurrentMenu = it;
    pMenus[it]->Update(); /** Render */
    this->pIO.InputHandler.CurrentMenu = 0;
  }
  for (int i = (int)FinalList.size() - 1; i >= 0; i--) {
    pIO.FDL.Merge(pMenus[FinalList[i]]->pLayout.GetDrawList());
  }
  pCurrentMenus.clear();
  pIO.Update();
  pIO.FDL.Optimize();
}

PD_API void Context::AboutMenu(bool* show) {
  if (auto m = BeginMenu("About UI7", UI7MenuFlags_Scrolling, show)) {
    m->Label("Palladium UI7 " + GetVersion());
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
      m->Label("sizeof(LI::Vertex) -> " + std::to_string(sizeof(Li::Vertex)));
      m->Label("__cplusplus -> " + std::to_string(__cplusplus));
      m->Label("Compiler -> {}",
               Strings::GetCompilerVersion());  // + LibInfo::CompiledWith());
    }
    EndMenu();
  }
}

PD_API void Context::MetricsMenu(bool* show) {
  if (auto m = BeginMenu("UI7 Metrics", UI7MenuFlags_Scrolling, show)) {
    m->Label("Palladium - UI7 " + GetVersion());
    m->Separator();
    m->Label(
        std::format("Average {:.3f} ms/f ({:.1f} FPS)",
                    ((float)pIO.DeltaStats.GetAverage() / 1000.f),
                    1000.f / ((float)pIO.DeltaStats.GetAverage() / 1000.f)));
    m->Label(std::format("NumVertices: {}", pIO.NumVertices));
    m->Label(std::format("NumIndices: {} -> {} Tris", pIO.NumIndices,
                         pIO.NumIndices / 3));
    m->Label("Menus: " + std::to_string(pMenus.size()));
    /** Section TimeTrace */
    m->SeparatorText("TimeTrace");
    if (m->BeginTreeNode("Traces (" +
                         std::to_string(PD::TT::GetTraceMap().size()) + ")")) {
      for (auto& it : PD::TT::GetTraceMap()) {
        if (m->BeginTreeNode(it.second.GetID())) {
          m->Label("Diff: " + UI7DTF(it.second.GetLastDiff()));
          m->Label("Protocol Len: " +
                   std::to_string(it.second.GetProtocol().GetLen()));
          m->Label("Average: " + UI7DTF(it.second.GetProtocol().GetAverage()));
          m->Label("Min: " + UI7DTF(it.second.GetProtocol().GetMin()));
          m->Label("Max: " + UI7DTF(it.second.GetProtocol().GetMax()));
          m->EndTreeNode();
        }
      }
      m->EndTreeNode();
    }
    m->SeparatorText("Palladium Info");
    m->Label("Os Driver: {}", PD::Os::GetDriverName());
    m->Label("Renderer: {}", PD::Gfx::GetDriverName());
    if (m->BeginTreeNode(
            std::string(std::string("Input: ") + PD::Hid::GetDriverName()))) {
      /* if (pIO.pCtx.Hid()->Flags & PD::HidDriver::Flags_HasKeyboard) {
         m->Label("- Keyboard Supported");
       }
       if (pIO.pCtx.Hid()->Flags & PD::HidDriver::Flags_HasMouse) {
         m->Label("- Mouse Supported");
       }
       if (pIO.pCtx.Hid()->Flags & PD::HidDriver::Flags_HasTouch) {
         m->Label("- Touch Supported");
       }
       if (pIO.pCtx.Hid()->Flags & PD::HidDriver::FLags_HasGamepad) {
         m->Label("- Gamepad Supported");
       }*/
      m->EndTreeNode();
    }
    /** Section IO */
    m->SeparatorText("IO");
    if (m->BeginTreeNode("Menus (" + std::to_string(pMenus.size()) + ")")) {
      for (auto& it : pMenus) {
        if (m->BeginTreeNode(it.second->pID.GetName())) {
          m->Label("Name: " + it.second->pID.GetName());
          m->Label(std::format("Pos: {}", it.second->pLayout.GetPosition()));
          m->Label(std::format("Size: {}", it.second->pLayout.GetSize()));
          m->Label(std::format("WorkRect: {}", it.second->pLayout.WorkRect));
          m->Label(std::format("Cursor: {}", it.second->pLayout.Cursor));
          if (m->BeginTreeNode(
                  "ID Objects (" +
                  std::to_string(it.second->pLayout.IDObjects.size()) + ")")) {
            for (auto& jt : it.second->pLayout.IDObjects) {
              m->Label(std::format("{:08X}", jt->GetID()));
            }
            m->EndTreeNode();
          }
          m->EndTreeNode();
        }
      }
      m->EndTreeNode();
    }
    if (m->BeginTreeNode("Active Menus (" +
                         std::to_string(pCurrentMenus.size()) + ")")) {
      for (auto& it : pCurrentMenus) {
        if (m->BeginTreeNode(pMenus[it]->pID.GetName())) {
          m->Label("Name: " + pMenus[it]->pID.GetName());
          m->Label(std::format("Pos: {}", pMenus[it]->pLayout.GetPosition()));
          m->Label(std::format("Size: {}", pMenus[it]->pLayout.GetSize()));
          m->Label(std::format("WorkRect: {}", pMenus[it]->pLayout.WorkRect));
          m->Label(std::format("Cursor: {}", pMenus[it]->pLayout.Cursor));
          if (m->BeginTreeNode(
                  "ID Objects (" +
                  std::to_string(pMenus[it]->pLayout.IDObjects.size()) + ")")) {
            for (auto& jt : pMenus[it]->pLayout.IDObjects) {
              m->Label(std::format("{:08X}", jt->GetID()));
            }
            m->EndTreeNode();
          }
          m->EndTreeNode();
        }
      }
      m->EndTreeNode();
    }
    // Well this are Li Drawlists now and they do not count their stats (yet)
    /*if (m->BeginTreeNode("DrawLists (" +
                         std::to_string(pIO.DrawListRegestry.size()) + ")")) {
      for (auto &it : pIO.DrawListRegestry) {
        if (m->BeginTreeNode(it.First.GetName())) {
          m->Label("Vertices: " + std::to_string(it.Second->NumVertices));
          m->Label("Indices: " + std::to_string(it.Second->NumIndices));
          m->Label("Base Layer: " + std::to_string(it.Second->Base));
          m->EndTreeNode();
        }
      }
      m->EndTreeNode();
    }*/
    m->Label("io->Time: " + Strings::FormatMillis(pIO.Time.Get()));
    m->Label(std::format("Delta: {:.3f}", pIO.Delta));
    m->Label(std::format("Framerate: {:.2f}", pIO.Framerate));
    m->Label(std::format("Focused Menu: {:08X}", pIO.InputHandler.FocusedMenu));
    m->Label(
        std::format("Dragged Object: {:08X}", pIO.InputHandler.DraggedObject));
    m->Label(std::format("DragTime: {:.2f}s",
                         pIO.InputHandler.DragTime.GetSeconds()));
    m->Label(
        std::format("DragDestination: [{}]", pIO.InputHandler.DragDestination));
    m->Label(std::format("DragSource: [{}]", pIO.InputHandler.DragSourcePos));
    m->Label(std::format("DragPos: [{}]", pIO.InputHandler.DragPosition));
    m->Label(
        std::format("DragLastPos: [{}]", pIO.InputHandler.DragLastPosition));
    EndMenu();
  }
}

PD_API void UI7::Context::StyleEditor(bool* show) {
  if (auto m = BeginMenu("UI7 Style Editor", UI7MenuFlags_Scrolling, show)) {
    m->Label("Palladium - UI7 " + GetVersion() + " Style Editor");
    m->Separator();
    m->DragData("MenuPadding", (float*)&pIO.MenuPadding, 2, 0.f, 100.f);
    m->DragData("FramePadding", (float*)&pIO.FramePadding, 2, 0.f, 100.f);
    m->DragData("ItemSpace", (float*)&pIO.ItemSpace, 2, 0.f, 100.f);
    m->DragData("MinSliderSize", (float*)&pIO.MinSliderDragSize, 2, 1.f, 100.f);
    m->DragData("OverScroll Modifier", &pIO.OverScrollMod, 1, 0.01f,
                std::numeric_limits<float>::max(), 0.01f, 2);
    m->Checkbox("Menu Border", pIO.ShowMenuBorder);
    m->Checkbox("Frame Border", pIO.ShowFrameBorder);
    m->SeparatorText("Theme");
    if (m->Button("Dark")) {
      UI7::Theme::Default(pIO.Theme);
    }
    m->SameLine();
    if (m->Button("Flashbang")) {
      UI7::Theme::Flashbang(pIO.Theme);
    }
    /// Small trick to print without prefix
#define ts(x) m->ColorEdit(std::string(#x).substr(9), pIO.Theme.GetRef(x));
#define ts2(x)                                                                \
  m->DragData(std::string(#x).substr(9), (u8*)&pIO.Theme.GetRef(x), 4, (u8)0, \
              (u8)255);
    ts(UI7Color_Background);
    ts(UI7Color_Border);
    ts(UI7Color_Button);
    ts(UI7Color_ButtonDead);
    ts(UI7Color_ButtonActive);
    ts(UI7Color_ButtonHovered);
    ts(UI7Color_Text);
    ts(UI7Color_TextDead);
    ts(UI7Color_Header);
    ts(UI7Color_HeaderDead);
    ts(UI7Color_Selector);
    ts(UI7Color_Checkmark);
    ts(UI7Color_FrameBackground);
    ts(UI7Color_FrameBackgroundHovered);
    ts(UI7Color_Progressbar);
    ts(UI7Color_ListEven);
    ts(UI7Color_ListOdd);
    this->EndMenu();
  }
}
}  // namespace UI7
}  // namespace PD