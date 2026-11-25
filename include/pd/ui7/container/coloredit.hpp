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

namespace PD {
namespace UI7 {
/**
 * Color Swatch + Label (opens a ColorPicker popup when clicked)
 */
class PD_API ColorEdit : public Container {
 public:
  ColorEdit(const std::string& label, u32* color, UI7::IO& io) {
    this->label = label;
    this->color_ref = color;
    this->tdim = io.Font->GetTextBounds(label.c_str(), io.FontScale);
  }
  ~ColorEdit() = default;

  void HandleInput() override;
  void Draw() override;
  void Update() override;

  /** Returns true once after the swatch got clicked */
  bool OpenRequest() {
    bool r = open_req;
    open_req = false;
    return r;
  }
  /** Color when the popup was opened */
  u32& Backup() { return backup; }

 private:
  fvec2 tdim;
  u32* color_ref = nullptr;
  u32 backup = 0;
  std::string label;
  bool open_req = false;
};

/**
 * Color Picker (SV square + hue/alpha bars or RGBA sliders)
 */
class PD_API ColorPicker : public Container {
 public:
  ColorPicker(u32* color, const u32* backup = nullptr) {
    color_ref = color;
    this->backup = backup;
  }
  ~ColorPicker() = default;

  void HandleInput() override;
  void Draw() override;
  void Update() override;

  void SetBackup(const u32* v) { backup = v; }

 private:
  void pCalc();
  void pSync();
  void pApplyHsv();

  u32* color_ref = nullptr;
  const u32* backup = nullptr;
  u32 last = 0;
  bool synced = false;
  float h = 0.f, s = 0.f, v = 0.f;
  // Geometry (absolute, calculated by pCalc)
  fvec4 rTab[2];
  fvec4 rSV, rHue, rAlpha;
  fvec4 rBar[4];
  fvec4 rOld, rNew;
  fvec2 pHex;
  float vw = 0.f;
};
}  // namespace UI7
}  // namespace PD
