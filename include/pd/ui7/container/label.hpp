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
/**
 * Label [Text] Object
 */
class Label : public Container {
 public:
  /**
   * Constructor for Label Object
   * @param label Label [Text] to Draw
   * @param pos Base Position
   * @param lr Renderer Reference
   */
  Label(const std::string& label, vec2 pos, LI::Renderer::Ref lr) {
    this->screen = lr->CurrentScreen();
    this->label = label;
    this->SetPos(pos);
    this->tdim = lr->GetTextDimensions(label);
    color = UI7Color_Text;
    this->SetSize(tdim);
  }
  ~Label() = default;

  /**
   * Override for the Rendering Handler
   * @note This function is usally called by Menu::Update
   * */
  void Draw() override;

 private:
  vec2 tdim;          ///< Text Size
  UI7Color color;     ///< Color
  std::string label;  ///< Text to Render
};
}  // namespace UI7
}  // namespace PD