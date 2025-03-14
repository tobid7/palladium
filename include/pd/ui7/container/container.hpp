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

#include <pd/core/common.hpp>
#include <pd/core/strings.hpp>
#include <pd/core/vec.hpp>
#include <pd/drivers/hid.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/io.hpp>

namespace PD {
namespace UI7 {
/**
 * Container base class all Objects are based on
 * @note this class can be used to create custom Objects as well
 */
class Container : public SmartCtor<Container> {
 public:
  Container() = default;
  /**
   * Constructor with pos and Size
   * @param pos Container Position
   * @param size Container Size
   */
  Container(const vec2& pos, const vec2& size) : pos(pos), size(size) {}
  /**
   * Constructor by a vec4 box
   * @param box Box containing top left and bottom right coords
   */
  Container(const vec4& box) : pos(box.xy()), size(box.zw() - box.xy()) {}
  ~Container() = default;

  /**
   * Init Function Required by every Object that uses
   * Render or Input functions
   * @param io IO Reference
   * @param l DrawList Reference
   */
  void Init(UI7::IO::Ref io, UI7::DrawList::Ref l) {
    list = l;
    this->io = io;
    this->screen = io->Ren->CurrentScreen();
  }

  /** Setter for Position */
  void SetPos(const vec2& pos) { this->pos = pos; }
  /** Setter for Size */
  void SetSize(const vec2& size) { this->size = size; }
  /** Getter for Position */
  vec2 GetPos() { return pos; }
  /** Getter for Size */
  vec2 GetSize() { return size; }
  /**
   * Get the Containers Final Position
   * for Rendering and Input (if it has a parent Object)
   */
  vec2 FinalPos() {
    vec2 res = pos;
    if (parent) {
      /// Probably should use parant->FinalPos here
      res += parent->GetPos();
    }
    return res;
  }

  /** Setter for Parent Container */
  void SetParent(Container::Ref v) { parent = v; }
  /** Getter for Parent Container */
  Container::Ref GetParent() { return parent; }

  /** Check if Rendering can be skipped */
  bool Skippable() const { return skippable; }
  /** Check if the Object got a timeout (ID OBJ Relevant) */
  bool Removable() const { return rem; }

  /**
   * Handles Scrolling by scrolling pos as well as
   * Time for Remove for ID Objects
   * @param scrolling Scrolling Position
   * @param viewport Viewport to check if the Object is skippable
   */
  void HandleScrolling(vec2 scrolling, vec4 viewport);
  /** Template function for Input Handling */
  virtual void HandleInput() {}
  /** Tamplate function for Object rendering */
  virtual void Draw() {}
  /** Template function to update internal data (if needed) */
  virtual void Update() {}

  /**
   * Function to unlock Input after Rendering is done in
   * Menu::Update
   * @note This is used if the Object got Input Handled directly after creation
   * to not check for Inputs twice
   */
  void UnlockInput() { inp_done = false; }

  /** Get the Objects ID (if it is an ID object)*/
  u32 GetID() const { return id; }
  /**
   * Set ID for ID Objects
   * @param id Object ID (hashed prefix+objname+prefixed_counter)
   */
  void SetID(u32 id) { this->id = id; }

 protected:
  /** used to skip Input/Render preocessing ot not*/
  bool skippable = false;
  /** value to check if an ID Object goes out of lifetime*/
  bool rem = false;
  /** Time of the last use (set by HandleScrolling)*/
  u64 last_use = 0;
  /** Input done or not for current frame*/
  bool inp_done = false;
  /** Reference to the Screen to draw the Object on*/
  Screen::Ref screen;
  /** Container Position*/
  vec2 pos;
  /** Container Size*/
  vec2 size;
  /** Reference to the Drawlist to Draw to*/
  UI7::DrawList::Ref list;
  /** IO Reference for Renderer and Theme */
  UI7::IO::Ref io;
  /** Reference to the parent container*/
  Container::Ref parent;
  /** Object ID (0 if unused)*/
  u32 id = 0;
};
}  // namespace UI7
}  // namespace PD
