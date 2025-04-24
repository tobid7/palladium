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
 * DragData Object can take a datatype or a list
 * and modifys these by moving left or right when dragging
 */
template <typename T>
class PD_UI7_API DragData : public Container {
 public:
  /**
   * Constructor
   * @param label Label of the Button
   * @param data Data reference (Supported types can be seen in dragdata.cpp)
   * @param num_elms Number of Array elements (for exaple with use ofvec4)
   * @param io IO Reference
   * @param min minimum number using Minimum limit
   * @param max Maximum number set by max limit by default
   * @param step To set the modifier for drag movement
   * @param precision for float and double to set precision
   */
  DragData(const std::string& label, T* data, size_t num_elms, UI7::IO::Ref io,
           T min = std::numeric_limits<T>::min(),
           T max = std::numeric_limits<T>::max(), T step = 1,
           int precision = 1) {
    // PD::Assert(data != nullptr, "Input Data Address is null!");
    this->label = label;
    this->data = data;
    this->elm_count = num_elms;
    this->min = min;
    this->max = max;
    this->step = step;
    this->precision = precision;
    this->tdim = io->Font->GetTextBounds(label, io->FontScale);
  }
  ~DragData() = default;

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
  fvec2 tdim;         ///< Text size
  std::string label;  ///< Label of the Button
  T* data;
  size_t elm_count = 0;
  T min;
  T max;
  T step;
  int precision = 1;
};
}  // namespace UI7
}  // namespace PD