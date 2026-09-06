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
#include <pd/ui7/io.hpp>
#include <pd/ui7/layout.hpp>

namespace PD {
namespace UI7 {
/**
 * Color Editor (Creating a PopUP when clicking)
 */
class PD_API ColorEdit : public Container {
 public:
  /**
   * Constructor
   * @param label Label of the Button
   * @param pos Base Position
   * @param lr Reference to the Renderer
   */
  ColorEdit(const std::string& label, u32* color, UI7::IO::Ref io) {
    // PD::Assert(color != nullptr, "Input Color Address is null!");
    this->label = label;
    this->color_ref = color;
    this->initial_color = *color;
    this->tdim = io->Font->GetTextBounds(label, io->FontScale);
  }
  ~ColorEdit() = default;
  PD_SHARED(ColorEdit);

  /**
   * Override for the Input Handler
   * @note This function is usally called by Menu::Update
   */
  void HandleInput() override;
  /**
   * Override for the Rendering Handler
   * @note This function is usally called by Menu::Update
   * */
  void Draw() override;

  /** Function to Update Size if framepadding changes */
  void Update() override;

 private:
  fvec2 tdim;                ///< Text size
  u32* color_ref = nullptr;  ///< Color Reference
  u32 initial_color;         ///< Initial Color
  std::string label;         ///< Label of the Button
  Layout::Ref layout;        ///< Layout to open
  bool is_shown = false;     ///< AHow Layout Editor
};
}  // namespace UI7
}  // namespace PD