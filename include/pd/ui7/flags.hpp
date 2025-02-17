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

using UI7MenuFlags = unsigned int;
using UI7Align = unsigned int;

enum UI7MenuFlags_ {
  UI7MenuFlags_None = 0,
  UI7MenuFlags_NoTitlebar = 1 << 0,
  UI7MenuFlags_CenterTitle = 1 << 1,
  UI7MenuFlags_HzScrolling = 1 << 2,
  UI7MenuFlags_VtScrolling = 1 << 3,
  UI7MenuFlags_NoBackground = 1 << 4,
  UI7MenuFlags_Scrolling = UI7MenuFlags_HzScrolling | UI7MenuFlags_VtScrolling,
};

/// @brief Probably need to update this
enum UI7Align_ {
  UI7Align_Left = 1 << 0,
  UI7Align_Center = 1 << 1,
  UI7Align_Right = 1 << 2,
  UI7Align_Top = 1 << 3,
  UI7Align_Mid = 1 << 4,
  UI7Align_Bottom = 1 << 5,
  UI7Align_Default = UI7Align_Left | UI7Align_Top,
};