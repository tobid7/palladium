#pragma once

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

#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
/**
 * Label [Text] Object
 */
class PD_API Label : public Container {
 public:
  Label() {}
  /**
   * Constructor for Label Object
   * @param label Label [Text] to Draw
   * @param lr Renderer Reference
   */
  Label(const std::string& label, IO& io) {
    this->label = label;
    this->tdim = io.Font->GetTextBounds(label.c_str(), io.FontScale);
    this->SetSize(tdim);
  }
  ~Label() = default;

  /**
   * Override for the Rendering Handler
   * @note This function is usally called by Menu::Update
   * */
  void Draw() override;
  /**
   * Override Update func to support Text modifications
   */
  void Update() override;

  void Reset() override {
    Container::Reset();
    tdim = 0;
    color = UI7Color_Text;
    label.clear();
  }

 private:
  fvec2 tdim;                      ///< Text Size
  UI7Color color = UI7Color_Text;  ///< Color
  std::string label;               ///< Text to Render
};
}  // namespace UI7
}  // namespace PD