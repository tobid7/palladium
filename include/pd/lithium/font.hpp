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
#include <pd/lithium/rect.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/maths/vec.hpp>

namespace PD {
namespace LI {
class Font : public SmartCtor<Font> {
 public:
  class Codepoint {
   public:
    Codepoint() {}
    ~Codepoint() {}

    u32 cp() const { return m_cp; }
    Codepoint& cp(u32 v) {
      m_cp = v;
      return *this;
    }
    vec4 uv() const { return m_uv; }
    Codepoint& uv(const vec4& v) {
      m_uv = v;
      return *this;
    }
    Texture::Ref tex() const { return m_tex; }
    Codepoint& tex(Texture::Ref v) {
      m_tex = v;
      return *this;
    }
    vec2 size() const { return m_size; }
    Codepoint& size(const vec2& v) {
      m_size = v;
      return *this;
    }
    float off() const { return m_off; }
    Codepoint& off(float v) {
      m_off = v;
      return *this;
    }
    bool invalid() const { return m_invalid; }
    Codepoint& invalid(bool v) {
      m_invalid = v;
      return *this;
    }

   private:
    u32 m_cp = 0;
    vec4 m_uv;
    Texture::Ref m_tex = nullptr;
    vec2 m_size;
    float m_off = 0;
    bool m_invalid = false;
  };
  Font() {}
  ~Font() {}
  void LoadTTF(const std::string& path, int px_height = 32);
  void LoadSystemFont();
  int PixelHeight() const { return pixel_height; }
  Codepoint& GetCodepoint(u32 c);
  bool SystemFont() const { return sysfont; }

 private:
  bool sysfont;
  int pixel_height;
  std::vector<Texture::Ref> textures;
  std::map<u32, Codepoint> cpmap;
};
}  // namespace LI
}  // namespace PD