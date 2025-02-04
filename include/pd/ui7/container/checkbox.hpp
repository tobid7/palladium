#pragma once

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

#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
class Checkbox : public Container {
 public:
  Checkbox(const std::string& label, vec2 pos, bool& usr_ref,
           LI::Renderer::Ref lr)
      : usr_ref(usr_ref) {
    this->screen = lr->CurrentScreen();
    this->label = label;
    this->SetPos(pos);
    this->tdim = lr->GetTextDimensions(label);
    color = UI7Color_FrameBackground;
    this->SetSize(cbs + vec2(tdim.x() + 5, 0));
  }
  ~Checkbox() {}

  void HandleInput(Hid::Ref inp) override;
  void Draw() override;

 private:
  vec2 tdim;
  vec2 cbs = vec2(18);
  UI7Color color;
  std::string label;
  bool& usr_ref;
};
}  // namespace UI7
}  // namespace PD