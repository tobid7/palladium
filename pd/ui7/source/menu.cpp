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

#include <pd/ui7/containers.hpp>
#include <pd/ui7/menu.hpp>

namespace PD {
namespace UI7 {
Menu::Menu(const ID &id, IO::Ref io) : pIO(io), pID(id) {
  pLayout = Layout::New(id, io);
  TitleBarHeight = io->FontScale * 30.f;
  pLayout->WorkRect.y += TitleBarHeight;
  pLayout->CursorInit();
}

PD_UI7_API void Menu::Label(const std::string &label) {
  // Layout API
  auto r = Label::New(label, pIO);
  pLayout->AddObject(r);
}

PD_UI7_API bool Menu::Button(const std::string &label) {
  bool ret = false;
  u32 id = Strings::FastHash("btn" + label +
                             std::to_string(pLayout->Objects.Size()));
  Container::Ref r = pLayout->FindObject(id);
  if (!r) {
    r = Button::New(label, pIO);
    r->SetID(id);
  }
  pLayout->AddObject(r);
  if (!r->Skippable()) {
    ret = std::static_pointer_cast<UI7::Button>(r)->IsPressed();
  }
  return ret;
}

PD_UI7_API void Menu::Checkbox(const std::string &label, bool &v) {
  u32 id = Strings::FastHash("cbx" + label +
                             std::to_string(pLayout->Objects.Size()));
  Container::Ref r = pLayout->FindObject(id);
  if (!r) {
    r = Checkbox::New(label, v, pIO);
    r->SetID(id);
  }
  pLayout->AddObject(r);
}

PD_UI7_API void Menu::Image(Li::Texture::Ref img, fvec2 size, Li::Rect uv) {
  Container::Ref r = Image::New(img, size, uv);
  pLayout->AddObject(r);
}

PD_UI7_API void Menu::Separator() {
  // Dynamic Objects are very simple...
  Container::Ref r = DynObj::New(
      [=, this](UI7::IO::Ref io, Li::DrawList::Ref l, UI7::Container *self) {
        l->DrawRectFilled(self->FinalPos(), self->GetSize(),
                          pIO->Theme->Get(UI7Color_TextDead));
      });
  // Set size before pushing (cause Cursor Move will require it)
  r->SetSize(fvec2(pLayout->Size.x - 10, 1));
  pLayout->AddObject(r);
}

PD_UI7_API void Menu::SeparatorText(const std::string &label) {
  // Also note to use [=] instead of [&] to not undefined access label
  Container::Ref r = DynObj::New([=, this](UI7::IO::Ref io, Li::DrawList::Ref l,
                                           UI7::Container *self) {
    fvec2 size = self->GetSize();
    fvec2 tdim = io->Font->GetTextBounds(label, io->FontScale);
    fvec2 pos = self->FinalPos();
    auto align = pLayout->GetAlignment();
    vec2 rpos = pLayout->AlignPosition(
        pos, tdim, fvec4(pLayout->Pos, pLayout->Size), align);
    if (!(align & UI7Align_Left)) {
      l->DrawRectFilled(fvec2(rpos.x + io->FramePadding.x, tdim.y * 0.5),
                        fvec2(pos.x - rpos.x - io->MenuPadding.x, 1),
                        io->Theme->Get(UI7Color_TextDead));
    }
    if (!(align & UI7Align_Right)) {
      l->DrawRectFilled(pos + fvec2(tdim.x + io->FramePadding.x, tdim.y * 0.5),
                        fvec2(size.x - tdim.x - io->MenuPadding.x, 1),
                        io->Theme->Get(UI7Color_TextDead));
    }
    l->DrawTextEx(rpos, label, io->Theme->Get(UI7Color_Text), 0,
                  fvec2(pLayout->Size.x, self->GetSize().y));
  });
  // Set size before pushing (cause Cursor Move will require it)
  r->SetSize(
      fvec2(pLayout->Size.x - 10, pIO->Font->PixelHeight * pIO->FontScale));
  pLayout->AddObject(r);
}
PD_UI7_API void Menu::HandleFocus() {
  // Check if menu can be focused for Selective Menu Input API
  vec4 newarea = fvec4(pLayout->Pos, pLayout->Size);
  if (!pIsOpen) {
    newarea = fvec4(pLayout->Pos, fvec2(pLayout->Size.x, TitleBarHeight));
  }
  if (Hid::IsDown(Hid::Key::Touch) &&
      Li::Renderer::InBox(Hid::MousePos(), newarea) &&
      !Li::Renderer::InBox(Hid::MousePos(),
                           pIO->InputHandler->FocusedMenuRect)) {
    pIO->InputHandler->FocusedMenu = pID;
  }
  if (pIO->InputHandler->FocusedMenu == pID) {
    pIO->InputHandler->FocusedMenuRect = newarea;
  }
}
PD_UI7_API void Menu::HandleScrolling() {}
PD_UI7_API void Menu::HandleTitlebarActions() {
  // Collapse
  if (!(Flags & UI7MenuFlags_NoCollapse)) {
    vec2 cpos = pLayout->Pos + pIO->FramePadding;
    // clr_collapse_tri = UI7Color_FrameBackground;
    if (pIO->InputHandler->DragObject(UI7::ID(pID.GetName() + "clbse"),
                                      fvec4(cpos, fvec2(18, TitleBarHeight)))) {
      if (pIO->InputHandler->DragReleased) {
        pIsOpen = !pIsOpen;
      }
      // clr_collapse_tri = UI7Color_FrameBackgroundHovered;
    }
  }
  // Close Logic
  if (!(Flags & UI7MenuFlags_NoClose) && pIsShown != nullptr) {
    vec2 cpos =
        fvec2(pLayout->Pos.x + pLayout->Size.x - 12 - pIO->FramePadding.x,
              pLayout->Pos.y + pIO->FramePadding.y);

    // clr_close_btn = UI7Color_FrameBackground;
    if (pIO->InputHandler->DragObject(UI7::ID(pID.GetName() + "clse"),
                                      fvec4(cpos, fvec2(12)))) {
      if (pIO->InputHandler->DragReleased) {
        *pIsShown = !(*pIsShown);
      }
      // clr_close_btn = UI7Color_FrameBackgroundHovered;
    }
  }
  // Menu Movement
  if (!(Flags & UI7MenuFlags_NoMove)) {
    if (pIO->InputHandler->DragObject(
            pID.GetName() + "tmv",
            fvec4(pLayout->Pos, fvec2(pLayout->Size.x, TitleBarHeight)))) {
      if (pIO->InputHandler->DragDoubleRelease) {
        pIsOpen = !pIsOpen;
      }
      pLayout->Pos = pLayout->Pos + (pIO->InputHandler->DragPosition -
                                     pIO->InputHandler->DragLastPosition);
      // Have no ViewPort Yet :(
      // pLayout->Pos = std::clamp(pLayout->Pos, fvec2(10), fvec2(1270, 710));
    }
  }
}
PD_UI7_API void Menu::DrawBaseLayout() {
  if (pIsOpen) {
    Container::Ref r = DynObj::New([](IO::Ref io, Li::DrawList::Ref l,
                                      UI7::Container *self) {
      l->Layer = 0;
      l->PathRectEx(self->FinalPos(), self->FinalPos() + self->GetSize(), 10.f,
                    LiPathRectFlags_KeepTop | LiPathRectFlags_KeepBot);
      l->PathFill(io->Theme->Get(UI7Color_Background));
      /*l->DrawRectFilled(self->FinalPos(), self->GetSize(),
                        io->Theme->Get(UI7Color_Background));*/
    });
    // Set size before pushing (cause Cursor Move will require it)
    r->SetSize(
        fvec2(pLayout->GetSize().x, pLayout->GetSize().y - TitleBarHeight));
    r->SetPos(fvec2(0, TitleBarHeight));
    pLayout->AddObjectEx(r, UI7LytAdd_NoCursorUpdate |
                                UI7LytAdd_NoScrollHandle | UI7LytAdd_Front);
  }
  if (!(Flags & UI7MenuFlags_NoTitlebar)) {
    Container::Ref r = DynObj::New(
        [=, this](UI7::IO::Ref io, Li::DrawList::Ref l, UI7::Container *self) {
          l->Layer = 20;
          /** Header Bar */
          l->DrawRectFilled(self->FinalPos(), self->GetSize(),
                            io->Theme->Get(UI7Color_Header));
          l->Layer = 21;
          /** Inline if statement to shift the Text if collapse sym is shown */
          /** What the hell is this code btw (didn't found a better way) */
          l->DrawText(self->FinalPos() + fvec2(Flags & UI7MenuFlags_NoClose
                                                   ? 0
                                                   : (TitleBarHeight -
                                                      pIO->FramePadding.y * 2 +
                                                      (io->FramePadding.x * 2)),
                                               0),
                      pID.GetName(), io->Theme->Get(UI7Color_Text));
        });
    r->SetSize(fvec2(pLayout->GetSize().x, TitleBarHeight));
    r->SetPos(0);
    pLayout->AddObjectEx(r,
                         UI7LytAdd_NoCursorUpdate | UI7LytAdd_NoScrollHandle);

    /** Collapse Sym */
    if (!(Flags & UI7MenuFlags_NoCollapse)) {
      r = DynObj::New([=, this](UI7::IO::Ref io, Li::DrawList::Ref l,
                                UI7::Container *self) {
        /** This sym actually requires layer 21 (i dont know why) */
        l->Layer = 21;
        /**
         * Symbol (Position Swapping set by pIsOpen ? openpos : closepos;)
         */
        l->DrawTriangleFilled(
            self->FinalPos(),
            self->FinalPos() +
                fvec2(self->GetSize().x, pIsOpen ? 0 : self->GetSize().y * 0.5),
            self->FinalPos() +
                fvec2(pIsOpen ? self->GetSize().x * 0.5 : 0, self->GetSize().y),
            io->Theme->Get(UI7Color_FrameBackground));
      });
      r->SetSize(TitleBarHeight - pIO->FramePadding.y * 2);
      r->SetPos(pIO->FramePadding);
      pLayout->AddObjectEx(r,
                           UI7LytAdd_NoCursorUpdate | UI7LytAdd_NoScrollHandle);
    }
    /** Close Sym (only shown if pIsShown is not nullptr) */
    if (!(Flags & UI7MenuFlags_NoClose) && pIsShown) {
      fvec2 size = TitleBarHeight - pIO->FramePadding.y * 2; // Fixed quad size
      // Need to clamp this way as the math lib lacks a less and greater
      // operator in vec2 (don't checked if it would make sense yet)
      size.x = std::clamp(size.x, 5.f, std::numeric_limits<float>::max());
      size.y = std::clamp(size.y, 5.f, std::numeric_limits<float>::max());
      // Probably should fix the minsize to be locked on y
      fvec2 cpos =
          fvec2(pLayout->Pos.x + pLayout->Size.x - size.x - pIO->FramePadding.x,
                pLayout->Pos.y + pIO->FramePadding.y);
      pLayout->DrawList->DrawLine(cpos, cpos + size,
                                  pIO->Theme->Get(UI7Color_FrameBackground), 2);
      pLayout->DrawList->DrawLine(cpos + fvec2(0, size.y),
                                  cpos + fvec2(size.x, 0),
                                  pIO->Theme->Get(UI7Color_FrameBackground), 2);
    }
  }
}
PD_UI7_API void Menu::Update() {
  HandleFocus();
  if (!(Flags & UI7MenuFlags_NoTitlebar)) {
    HandleTitlebarActions();
  }
  DrawBaseLayout();
  pLayout->Update();
}
} // namespace UI7
} // namespace PD
