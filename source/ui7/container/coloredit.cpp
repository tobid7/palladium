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

#include <pd/ui7/container/coloredit.hpp>
#include <pd/ui7/containers.hpp>

namespace PD {
namespace UI7 {
PD_API void ColorEdit::HandleInput() {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  // Assert(screen.get(), "Screen is not set up!");
  // if (screen->ScreenType() == Screen::Bottom) {
  if (io->InputHandler.DragObject(this->GetID() + 2, fvec4(FinalPos(), size))) {
    if (io->InputHandler.DragReleasedAW) {
      is_shown = !is_shown;
    }
  }
  //}
  inp_done = true;
}
PD_API void ColorEdit::Draw() {
  // Assert(io.get() && list.get(), "Did you run Container::Init correctly?");
  // io->Ren->OnScreen(screen);
  list->PathRect(FinalPos(), FinalPos() + io->ItemRowHeight, io->FrameRounding);
  list->PathFill(*color_ref);
  list->DrawText(FinalPos() + fvec2(io->ItemSpace.x + io->ItemRowHeight, 0),
                 label.c_str(), io->Theme.Get(UI7Color_Text));
  if (is_shown) {
    if (!layout) {
      layout = new UI7::Layout(GetID(), *io);
    }
    layout->SetPosition(FinalPos());
    DynObj* r = io->DynObjPool.Allocate();
    *r = UI7::DynObj([=, this](UI7::IO* io, Li::Drawlist* l, Container* thiz) {
      thiz->SetSize(layout->GetSize());
      // l->Layer(30);
      l->PathRect(thiz->GetPos(), thiz->GetPos() + thiz->GetSize(),
                  io->FrameRounding);
      l->PathFill(io->Theme.Get(UI7Color_FrameBackground));
    });
    layout->AddObjectEx(r, UI7LytAdd_Front | UI7LytAdd_NoCursorUpdate |
                               UI7LytAdd_NoScrollHandle);
    r = io->DynObjPool.Allocate();
    *r = UI7::DynObj([=, this](UI7::IO* io, Li::Drawlist* l, Container* thiz) {
      l->PathRect(thiz->FinalPos(), thiz->FinalPos() + io->ItemRowHeight,
                  io->FrameRounding);
      l->PathFill(*color_ref);
      l->DrawText(
          thiz->FinalPos() + fvec2(io->ItemSpace.x + io->ItemRowHeight, 0),
          label.c_str(), io->Theme.Get(UI7Color_Text));
    });
    r->SetSize(PD::fvec2(200, io->ItemRowHeight));
    layout->AddObject(r);
    layout->Label("RGBA: ({}, {}, {}, {})", *((u8*)color_ref),
                  *(((u8*)color_ref) + 1), *(((u8*)color_ref) + 2),
                  *(((u8*)color_ref) + 3));
    layout->Label("Hex: {}", PD::Color(*color_ref).Hex(true));

    layout->Slider<u8>("R", ((u8*)color_ref));
    layout->Slider<u8>("G", ((u8*)color_ref) + 1);
    layout->Slider<u8>("B", ((u8*)color_ref) + 2);
    layout->Slider<u8>("A", ((u8*)color_ref) + 3);
    layout->Update();
    list->Merge(layout->GetDrawList());
    // io->RegisterDrawList(GetID(), layout->GetDrawList());
  }
}

PD_API void ColorEdit::Update() {
  // Assert(io.get(), "Did you run Container::Init correctly?");
  this->SetSize(
      fvec2(tdim.x + io->ItemSpace.x + io->ItemRowHeight, io->ItemRowHeight));
}
}  // namespace UI7
}  // namespace PD