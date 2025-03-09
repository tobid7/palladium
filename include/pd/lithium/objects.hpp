#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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
#include <pd/core/vec.hpp>
#include <pd/lithium/command.hpp>
#include <pd/lithium/flags.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/lithium/vertex.hpp>

namespace PD {
namespace LI {
class Renderer;
/**
 * Prerendered Object that can be edidet at runtime
 * and supports diffrent textures as well
 */
class StaticObject : public SmartCtor<StaticObject> {
 public:
  StaticObject() = default;
  ~StaticObject() = default;

  /**
   * Push a Draw Command to the Static Object
   * @param v Command to add
   */
  void PushCommand(Command::Ref v) { cmds.push_back(v); }

  /**
   * Copy the root Object to a copy buffer for modification
   */
  void ReCopy() {
    cpy.clear();
    for (auto it : cmds) {
      cpy.push_back(Command::New(it));
    }
  }

  /**
   * Modify the Color of a specific Command in the List
   * @param idx Index of the Command
   * @param col new COlor to replace with
   */
  void ReColorQuad(int idx, u32 col) {
    if (idx > (int)cpy.size()) {
      return;
    }
    for (auto& it : cpy[idx]->VertexList()) {
      it.Color(col);
    }
  }

  /**
   * Move the every Command in the Object by a specific offset
   * @param off offset position to apply
   */
  void MoveIt(vec2 off) {
    for (auto& it : cpy) {
      for (auto& jt : it->VertexList()) {
        jt.pos += off;
      }
    }
  }

  /**
   * Change Color of all Commands in the List
   * @param col new Color to use
   */
  void ReColor(u32 col) {
    for (auto& it : cpy) {
      for (auto& jt : it->VertexList()) {
        jt.Color(col);
      }
    }
  }

  /**
   * Assign a New Layer to all Objects
   * @param layer new layer to set to all its commands
   */
  void ReLayer(int layer) {
    for (auto& it : cpy) {
      it->Layer(layer);
    }
  }

  /**
   * Change the Commands Index by setting a start index
   * @param start Start index position
   */
  void ReIndex(int start) {
    for (int i = 0; i < (int)cpy.size(); i++) {
      cpy[i]->Index(start + i);
    }
  }

  /**
   *  Set a Custom Scissor Mode for Object Copy List
   * @param m New Mode to Set
   */
  void ReSetScissorMode(ScissorMode m) {
    for (auto& i : cpy) {
      i->SetScissorMode(m);
    }
  }

  /**
   * Set Custom Scissor Rect to All Objects
   * @param v Scissor Rect to set
   */
  void ReScissorRect(const vec4& v) {
    for (auto& i : cpy) {
      i->ScissorRect(v);
    }
  }

  /**
   * Get a Reference to the Copy Commands List
   * @return command list reference
   */
  std::vector<Command::Ref>& List() {
    if (cpy.size() != 0) {
      return cpy;
    }
    return cmds;
  }

 private:
  /** Copy Buffer */
  std::vector<Command::Ref> cpy;
  /** Source Buffer */
  std::vector<Command::Ref> cmds;
};
/**
 * Text Box container for TMS and AST
 */
class TextBox {
 public:
  TextBox() = default;
  /**
   * Baisc Constructor
   * @param s Size of the text
   * @param time creation time
   */
  TextBox(const vec2& s, float time) {
    size = s;
    time_created = time;
    optional = false;
  }
  ~TextBox() = default;

  /**
   * Setter for Time created
   * @param v time
   */
  void TimeCreated(float v) { time_created = v; }
  /**
   * Setter for Size
   * @param v size
   */
  void Size(const vec2& v) { size = v; }
  /**
   * Setter for is optional
   *
   * - optional text contains the result string of wrap/short
   * @param v optional
   */
  void Optional(bool v) { optional = v; }
  /**
   * set Result of Short Text or Wrap Text
   * @param v text
   */
  void Text(const std::string& v) { text = v; }

  /**
   * Get Size
   * @return size
   */
  vec2 Size() const { return size; }
  /**
   * Get Time Created / Updated
   * @param time created/updated
   */
  float TimeCreated() const { return time_created; }
  /**
   * Check if Optional or not
   * @return is optional
   */
  bool Optional() const { return optional; }
  /**
   * Get Optional Text
   * @return text
   */
  std::string Text() const { return text; }

 private:
  /** Text Size */
  vec2 size;
  /** Time Created / Updated */
  float time_created;
  /** Is Optional */
  bool optional;
  /** Text Wrap / Short */
  std::string text;
};
/**
 * Static Text [abillity to Prerender Texts into a list of Commands]
 */
class StaticText : public SmartCtor<StaticText> {
 public:
  StaticText() = default;
  /**
   * Wrap Constructor to Setup
   * @param ren Renderer direct pointer reference
   * @param pos Position
   * @param clr Color
   * @param text Text to Render
   * @param flags Text Flags
   * @param box Additional textbox for specific flags
   */
  StaticText(Renderer* ren, const vec2& pos, u32 clr, const std::string& text,
             LITextFlags flags = 0, const vec2& box = 0) {
    Setup(ren, pos, clr, text, flags, box);
  }
  ~StaticText() = default;
  /**
   * Function that Sets Up the Rendering
   * @param ren Renderer direct pointer reference
   * @param pos Position
   * @param clr Color
   * @param text Text to Render
   * @param flags Text Flags
   * @param box Additional textbox for specific flags
   */
  void Setup(Renderer* ren, const vec2& pos, u32 clr, const std::string& text,
             LITextFlags flags = 0, const vec2& box = 0);

  /**
   * Get Text Size
   * @return size
   */
  vec2 GetDim() const { return tdim; }
  /**
   * Get Text Posiotion
   * @return position
   */
  vec2 GetPos() const { return pos; }
  /**
   * Setter to Update Color
   * @param col color
   */
  void SetColor(u32 col);
  /**
   * Setter to raplace the texts Commands Position
   * @param pos new position
   */
  void SetPos(const vec2& pos);
  /**
   * Set Layer of the Text
   * @param l layer
   */
  void SetLayer(int l);

  /**
   * Function to unset the used variable
   */
  void SetUnused() { used = false; }
  /**
   * Function to check if the text got rendered
   * @return is used
   */
  bool Used() const { return used; }
  /**
   * Function to check if the text got setup
   * @return is setup
   */
  bool IsSetup() { return text != nullptr; }

  /**
   * Draw the Text
   */
  void Draw();

  /** Get the Raw Object for Custom API's */
  StaticObject::Ref GetRawObject() { return text; }

  /**
   * Set Font used by the static Text
   * @param fnt Font used
   */
  void Font(Font::Ref fnt) { font = fnt; }
  /**
   * Get Font used by the Text
   * @return Font Reference
   */
  Font::Ref Font() { return font; }

  /**
   *  Set a Custom Scissor Mode Static Text
   * @param m New Mode to Set
   */
  void SetScissorMode(ScissorMode m) { text->ReSetScissorMode(m); }

  /**
   * Set Custom Scissor Rect to Static Text
   * @param v Scissor Rect to set
   */
  void ScissorRect(const vec4& v) { text->ReScissorRect(v); }

 private:
  /** Font */
  Font::Ref font;
  /** Text got Rendered */
  bool used;
  /** Renderer pointer Reference */
  Renderer* ren;
  /** Text Size */
  vec2 tdim;
  /** Text Position */
  vec2 pos;
  /** Static Text Object */
  StaticObject::Ref text;
};
}  // namespace LI
}  // namespace PD