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
#include <pd/lithium/renderer.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
/** DrawList class */
class DrawList : public SmartCtor<DrawList> {
 public:
  /**
   * Constructor for a new Drawlist
   * @param r Renderer reference
   */
  DrawList(LI::Renderer::Ref r) { ren = r; }
  ~DrawList() = default;

  /**
   * Render a Rectangle
   * @param pos Position
   * @param szs Size
   * @param clr Color
   */
  void AddRectangle(vec2 pos, vec2 szs, const UI7Color& clr);
  /**
   * Render a Triangle
   * @param pos0 Position a
   * @param pos1 Position b
   * @param pos2 Position c
   * @param clr Color
   */
  void AddTriangle(vec2 pos0, vec2 pos1, vec2 pos2, const UI7Color& clr);
  /**
   * Render a Text
   * @param pos Position
   * @param text Text
   * @param clr Color
   * @param flags Flags
   * @param box Aditional Text Box limit (for specific flags)
   */
  void AddText(vec2 pos, const std::string& text, const UI7Color& clr,
               LITextFlags flags = 0, vec2 box = vec2());
  /**
   * Render an Image
   * @param pos Position
   * @param img Image Texture Reference
   * @param size Optional Size of the Image
   */
  void AddImage(vec2 pos, Texture::Ref img, vec2 size = 0.f);

  /** Clear the Drawlist */
  void Clear();
  /** Process [Render] the Drawlist */
  void Process();

  /** Getter for the Layer */
  int Layer() const { return layer; }
  /** Setter fot the Layer */
  void Layer(int v) { layer = v; }

 private:
  /** Base Layer offset (Internal Used) */
  int BaseLayer() const { return base; }
  /** Base Layer offset (Internal Used) */
  void BaseLayer(int v) { base = v; }

  // Set friendclass here to not expose private functions as public
  friend class Menu;
  friend class Context;

  int layer;              ///< Current Layer
  int base;               ///< Base Layer
  LI::Renderer::Ref ren;  ///< Renderer Reference
  // Map for Auto Static Text
  std::unordered_map<u32, LI::StaticText::Ref> static_text;
  // List of Drawcommands generated
  std::vector<std::pair<bool, LI::Command::Ref>> commands;
};
}  // namespace UI7
}  // namespace PD