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

#include <pd/core/core.hpp>
#include <pd/lithium/command.hpp>
#include <pd/lithium/rect.hpp>
#include <pd/lithium/texture.hpp>

using LIBackendFlags = PD::u32;
enum LIBackendFlags_ {
  LIBackendFlags_None = 0,
  LIBackendFlags_FlipUV_Y = 1 << 0,  // Essential for Font Loading
};

namespace PD {
namespace LI {
class Backend {
 public:
  Backend(const std::string& name = "NullBackend") : pName(name) {}
  ~Backend() = default;
  // Using Legacy SmartCTOR API here
  PD_SMART_CTOR(Backend)

  virtual void Init() {}
  virtual void Deinit() {}
  virtual void NewFrame() {}

  virtual void BindTexture(TexAddress addr) {}

  virtual void RenderDrawData(const Vec<Command::Ref>& Commands) {}

  virtual Texture::Ref LoadTexture(
      const std::vector<PD::u8>& pixels, int w, int h,
      Texture::Type type = Texture::Type::RGBA32,
      Texture::Filter filter = Texture::Filter::LINEAR) {
    // Texture loading not supported (when this func not get override)
    return nullptr;
  }

  /** Backend identification name */
  const std::string pName = "NullBackend";
  LIBackendFlags Flags = 0;
  ivec2 ViewPort;
  fvec4 ClearColor;
  // Optional Index Counter
  int IndexCounter = 0;
  // Optional Vertex Counter
  int VertexCounter = 0;
  // Optional Frame Counter
  int FrameCounter = 0;
};
}  // namespace LI
}  // namespace PD