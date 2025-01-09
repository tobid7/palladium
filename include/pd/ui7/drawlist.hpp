#pragma once

/*
MIT License
Copyright (c) 2024 René Amthor (tobid7)

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

#include <pd/common/common.hpp>
#include <pd/graphics/lithium.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
class UI7DrawList : SmartCtor<UI7DrawList> {
 public:
  UI7DrawList(LI::Renderer::Ref r) { ren = r; }
  ~UI7DrawList() = default;

  void AddRectangle(vec2 pos, vec2 szs, const UI7Color& clr);
  void AddTriangle(vec2 pos0, vec2 pos1, vec2 pos2, const UI7Color& clr);
  void AddText(vec2 pos, const std::string& text, const UI7Color& clr,
               LITextFlags flags = 0, vec2 box = vec2());
  void AddImage(vec2 pos, Texture::Ref img);

  void Clear();
  void Process();

  int Layer() const { return layer; }
  void Layer(int v) { layer = v; }

 private:
  int layer;
  LI::Renderer::Ref ren;
  std::vector<LI::Command::Ref> commands;
};
}  // namespace PD