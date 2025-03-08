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
#include <pd/ui7/io.hpp>

namespace PD {
namespace UI7 {
/**
 * Button Object
 * @note Button Press is delayed by 1 frame
 * (but the visual reaction is done in the same frame)
 * This only means that InPressed is responding the info in
 * the next frame
 */
class Button : public Container {
 public:
  /**
   * Button Object constructor
   * @param label Label of the Button
   * @param pos Base Position
   * @param lr Reference to the Renderer
   */
  Button(const std::string& label, UI7::IO::Ref io) {
    this->label = label;
    this->tdim = io->Ren->GetTextDimensions(label);
  }
  ~Button() = default;

  /** Return true if butten is pressed*/
  bool IsPressed() { return pressed; }
  /**
   * Override for the Input Handler
   * @note This function is usally called by Menu::Update
   * @param inp Reference to the Input Handler
   */
  void HandleInput(Hid::Ref inp) override;
  /**
   * Override for the Rendering Handler
   * @note This function is usally called by Menu::Update
   * */
  void Draw() override;

  /** Function to Update Size if framepadding changes */
  void Update() override;

 private:
  vec2 tdim;                         ///< Text size
  UI7Color color = UI7Color_Button;  ///< current button color
  std::string label;                 ///< Label of the Button
  bool pressed = false;              ///< ispressed value
};
}  // namespace UI7
}  // namespace PD