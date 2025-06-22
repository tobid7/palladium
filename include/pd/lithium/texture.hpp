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

#include <pd/core/core.hpp>
#include <pd/lithium/rect.hpp>

namespace PD {
namespace Li {
/** Use so address type for TexAddress */
using TexAddress = uintptr_t;
class Texture {
 public:
  /** Texture Types */
  enum Type {
    RGBA32,  ///< Rgba 32Bit
    RGB24,   ///< Rgb 24 Bit
    A8,      ///< A8 8Bit alpha
  };
  /** Texture Filters */
  enum Filter {
    NEAREST,  ///< Nearest
    LINEAR,   ///< Linear
  };
  /** Constructor */
  Texture() : Address(0), Size(0), UV(fvec4(0.f, 0.f, 1.f, 1.f)) {}
  Texture(TexAddress addr, ivec2 size,
          Li::Rect uv = fvec4(0.f, 0.f, 1.f, 1.f)) {
    Address = addr;
    Size = size;
    UV = uv;
  }

  PD_SHARED(Texture);

  void CopyFrom(Texture::Ref tex) {
    Address = tex->Address;
    Size = tex->Size;
    UV = tex->UV;
  }

  /** Left in Code getter (should be remoevd) */
  ivec2 GetSize() const { return Size; }
  Li::Rect GetUV() const { return UV; }

  operator ivec2() const { return Size; }
  operator Li::Rect() const { return UV; }

  TexAddress Address;
  ivec2 Size;
  Li::Rect UV;
};
}  // namespace Li
}  // namespace PD