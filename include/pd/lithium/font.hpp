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
#include <pd/lithium/rect.hpp>
#include <pd/lithium/texture.hpp>

namespace PD {
namespace LI {
/** Font Loader for Lithium */
class Font : public SmartCtor<Font> {
 public:
  /** Codepoint Data holder */
  class Codepoint {
   public:
    Codepoint() = default;
    ~Codepoint() = default;

    /**
     * Codepoint ID Getter
     * @return 32Bit codepoint value
     */
    u32 cp() const { return m_cp; }
    /**
     * Codepoint ID Setter
     * @param v codepoint id
     * @return DataHolder reference
     */
    Codepoint& cp(u32 v) {
      m_cp = v;
      return *this;
    }
    /**
     * Getter for the UV Coords
     * @return uv coords
     */
    vec4 uv() const { return m_uv; }
    /**
     * Setter for the UV Coords
     * @param v uv coords
     * @return DataHolder Reference
     */
    Codepoint& uv(const vec4& v) {
      m_uv = v;
      return *this;
    }
    /**
     * Getter for the Texture reference
     * @return Texture Reference
     */
    Texture::Ref tex() const { return m_tex; }
    /**
     * Setter for the Texture Reference
     * @param v Texture Reference
     * @return DataHolder Reference
     */
    Codepoint& tex(Texture::Ref v) {
      m_tex = v;
      return *this;
    }
    /**
     * Getter for the size
     * @return Size
     */
    vec2 size() const { return m_size; }
    /**
     * Setter for the Size
     * @param v size
     * @return DataHolder Reference
     */
    Codepoint& size(const vec2& v) {
      m_size = v;
      return *this;
    }
    /**
     * Getter for the Position offset
     * @return offset
     */
    float off() const { return m_off; }
    /**
     * Setter for the Render Offset
     * @param v offset
     * @return DataHolder Reference
     */
    Codepoint& off(float v) {
      m_off = v;
      return *this;
    }
    /**
     * Getter to check if Codepoint is invalid
     * @return true if invalid
     */
    bool invalid() const { return m_invalid; }
    /**
     * Setter for invald state
     * @param v true or false
     * @return DataHolder Reference
     */
    Codepoint& invalid(bool v) {
      m_invalid = v;
      return *this;
    }

   private:
    /** 32Bit Codepoint ID */
    u32 m_cp = 0;
    /** UvMap */
    vec4 m_uv;
    /** Texture Reference */
    Texture::Ref m_tex = nullptr;
    /** Codepoint Size */
    vec2 m_size;
    /** Render Position Offset */
    float m_off = 0;
    /** Invalid check (for skip in renderer) */
    bool m_invalid = false;
  };
  Font() = default;
  ~Font() = default;
  /**
   * Load a TTF File
   * @param path Path to the TTF file
   * @param px_height Pixelheight of the codepoints (limit by 64)
   */
  void LoadTTF(const std::string& path, int px_height = 32);
  /**
   * Load 3DS System Font
   */
  void LoadSystemFont();
  /**
   * Getter for PixelHeight
   * @return pixelheigt
   */
  int PixelHeight() const { return pixel_height; }
  /**
   * Getter for Codepoint reference
   * @return codepoint dataholder reference
   */
  Codepoint& GetCodepoint(u32 c);
  /**
   * Check if Systemfont is used
   * @return true if is system font
   */
  bool SystemFont() const { return sysfont; }

 private:
  /** sysfont set to true if LoadSystemFont got called */
  bool sysfont;
  /** Pixelheight */
  int pixel_height;
  /** List of textures (codepoints are using) */
  std::vector<Texture::Ref> textures;
  /** 32Bit Codepoint Dataholder Reference Map */
  std::map<u32, Codepoint> cpmap;
};
}  // namespace LI
}  // namespace PD