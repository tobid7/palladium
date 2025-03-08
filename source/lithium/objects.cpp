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

#include <pd/external/stb_truetype.h>

#include <pd/core/io.hpp>
#include <pd/core/strings.hpp>
#include <pd/core/sys.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/renderer.hpp>

namespace PD {
namespace LI {
void StaticText::Setup(Renderer* ren, const vec2& pos, u32 clr,
                       const std::string& text, LITextFlags flags,
                       const vec2& box) {
  this->tdim = ren->GetTextDimensions(text);
  this->pos = pos;
  this->ren = ren;
  this->text = StaticObject::New();
  /// Ensure that it also renders Out of Screen i guess
  ren->TextCommand(this->text->List(), pos, clr, text,
                   flags | LITextFlags_RenderOOS, box);
  Renderer::OptiCommandList(this->text->List());
  // Make sure to bring the text in edit mode
  // Fixes flickering problems in ui7
  this->text->ReCopy();
}

void StaticText::Draw() {
  used = true;
  for (auto& it : text->List()) {
    ren->PushCommand(it);
  }
  text->ReCopy();
}

void StaticText::SetColor(u32 col) { text->ReColor(col); }
void StaticText::SetPos(const vec2& pos) { text->MoveIt(pos - this->pos); }

void StaticText::SetLayer(int layer) { text->ReLayer(layer); }
}  // namespace LI
}  // namespace PD