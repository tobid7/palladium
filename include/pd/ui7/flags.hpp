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

/** 32Bit Value to Stpre Menu Flags */
using UI7MenuFlags = unsigned int;
/** 32Bit Value to store Alignment Flags */
using UI7Align = unsigned int;
/** 32Bit Value to store Context (IO) flags */
using UI7IOFlags = unsigned int;
/** 32Bit Value for Layout Flags */
using UI7LayoutFlags = unsigned int;

/** Menu Flags */
enum UI7MenuFlags_ {
  UI7MenuFlags_None = 0,               ///< No Flags (Default)
  UI7MenuFlags_NoTitlebar = 1 << 0,    ///< Dont Show Titlebar
  UI7MenuFlags_CenterTitle = 1 << 1,   ///< Center the Menu Title in Titlebar
  UI7MenuFlags_HzScrolling = 1 << 2,   ///< Enable Horizontal Scrolling
  UI7MenuFlags_VtScrolling = 1 << 3,   ///< Enable Vertical Scrolling
  UI7MenuFlags_NoBackground = 1 << 4,  ///< Dont Render Menu Background
  UI7MenuFlags_NoClipRect = 1 << 5,    ///< Disable clip render area of the Menu
  UI7MenuFlags_NoCollapse = 1 << 6,    ///< Disable Menu Collapse
  UI7MenuFlags_NoMove = 1 << 7,        ///< Disable Menu Movement
  UI7MenuFlags_NoResize = 1 << 8,      ///< Disable Menu Resize
  UI7MenuFlags_NoClose = 1 << 9,       ///< Disable Close Button
  UI7MenuFlags_NoScrollbar = 1 << 10,  ///< Hide the Scrollbar
  // POC
  UI7MenuFlags_Maximize = 1 << 11,        ///< Add a Maximize Button
  UI7MenuFlags_Minimize = 1 << 12,        ///< Add a Minimize Button
  UI7MenuFlags_AlwaysAutoSize = 1 << 13,  ///< Always Auto Resize Menu
  // Enable Horizontal and Vertical Scrolling
  UI7MenuFlags_Scrolling = UI7MenuFlags_HzScrolling | UI7MenuFlags_VtScrolling,
};

/** UI7 Layout Flags */
enum UI7LayoutFlags_ {
  UI7LayoutFlags_None = 0,              ///< No Flags used
  UI7LayoutFlags_UseClipRect = 1 << 0,  ///< Enable ClipRect
  UI7LayoutFlags_HzScrolling = 1 << 1,  ///< Scrolling Horizontal
  UI7LayoutFlags_VtScrolling = 1 << 2,  ///< Scrolling Vertical
};

/** UI7 Context Flags */
enum UI7IOFlags_ {
  UI7IOFlags_None = 0,                 ///< No Additional Config available
  UI7IOFlags_HasTouch = 1 << 0,        ///< Enable touch support [future]
  UI7IOFlags_HasMouseCursor = 1 << 1,  ///< Enable Mouse support [future]
};

/** Probably need to update this */
enum UI7Align_ {
  UI7Align_Left = 1 << 0,    ///< [Hz Op] Align Left (Default)
  UI7Align_Center = 1 << 1,  ///< [Hz Op] Align Center
  UI7Align_Right = 1 << 2,   ///< [Hz Op] Align Right
  UI7Align_Top = 1 << 3,     ///< [Vt Op] Align Top (Default)
  UI7Align_Mid = 1 << 4,     ///< [Vt Op] Align Mid
  UI7Align_Bottom = 1 << 5,  ///< [Vt Op] Align Bottom
  // Default Horizontal and Vertical Alignment
  UI7Align_Default = UI7Align_Left | UI7Align_Top,
};

/** Special flags for Layout::AddObjectEx */
enum UI7LytAdd_ {
  UI7LytAdd_None = 0,                 ///< Also known as default or ->AddObject
  UI7LytAdd_NoCursorUpdate = 1 << 0,  ///< Add without cursor alignment
  UI7LytAdd_NoScrollHandle = 1 << 1,  ///< Skip HandleScrolling
  UI7LytAdd_Front = 1 << 2,           ///< Add in front of the list
  UI7LytAdd_NoGlobalClipping = 1 << 3,  ///< Dont use global Layout clip rect
  UI7LytAdd_NoClipTitlebar = 1 << 4,  ///< Dont clip content behind the titlebar
};

/**
 * Todo: Look at this
 * Maybe proof of concept ???
 * Didnt remember that this exists
 */
enum UI7ContainerFlags_ {
  UI7ContainerFlags_None = 0,
  UI7ContainerFlags_EnableInternalInput = 1 << 0,
  UI7ContainerFlags_Selectable = 1 << 1,
  UI7ContainerFlags_OutlineSelected = 1 << 2,
};