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

#include <citro3d.h>

#include <pd/core/common.hpp>
#include <pd/lithium/rect.hpp>
#include <pd/core/vec.hpp>

namespace PD {
class Texture : public SmartCtor<Texture> {
 public:
  enum Type {
    RGBA32,
    RGB24,
    A8,
  };

  enum Filter {
    NEAREST,
    LINEAR,
  };
  /// @brief Default constructor
  Texture() : uv(0.f, 1.f, 1.f, 0.f) {}
  /// @brief Load file Constructor
  /// @param path path to file
  Texture(const std::string& path, bool t3x = false) : uv(0.f, 1.f, 1.f, 0.f) {
    if (t3x) {
      this->LoadT3X(path);
    } else {
      this->LoadFile(path);
    }
  }
  /// @brief Load Memory constructor
  /// @param data File Data reference
  Texture(const std::vector<u8>& data) : uv(0.f, 1.f, 1.f, 0.f) {
    this->LoadMemory(data);
  }
  /// @brief Load Pixels constructor
  /// @param data Pixel Buffer reference
  /// @param w width
  /// @param h height
  /// @param type Buffer Type
  /// @param filter Filter
  Texture(const std::vector<u8>& data, int w, int h, Type type = RGBA32,
          Filter filter = NEAREST)
      : uv(0.f, 1.f, 1.f, 0.f) {
    this->LoadPixels(data, w, h, type, filter);
  }
  /// @brief Deconstructor (aka auto delete)
  ~Texture() {
    if (autounload) {
      Delete();
    }
  }

  /// @brief Deletes image (if not already unloaded)
  void Delete();

  /// @brief Load a png/jpg/bmp from fs into a gpu tex
  /// @param path path to image file
  void LoadFile(const std::string& path);
  /// @brief Load a png/jpg/bmp from memory
  /// @param data reference to data buffer of the file
  void LoadMemory(const std::vector<u8>& data);
  /// @brief Create Texture out of Pixel Data
  /// @param data Data reference
  /// @param w width
  /// @param h heigt
  /// @param type Type of the databuffer
  /// @param filter Filter (NEAREST OR LINEAR)
  void LoadPixels(const std::vector<u8>& data, int w, int h, Type type = RGBA32,
                  Filter filter = NEAREST);

  /// @brief Load a texture of a T3X File
  /// @note This is used for single texture T3X
  /// Not for SpriteSheets
  /// @param path path to .t3x file
  void LoadT3X(const std::string& path);

  /// @brief Input a Texture that you had set up on your own
  /// @param tex Texture reference (deletes itself)
  /// @param rszs The size of the source image
  /// @param uvs Your uv Setup
  void LoadExternal(C3D_Tex* tex, vec2 rszs, LI::Rect uvs) {
    this->Delete();
    this->tex = tex;
    this->size = rszs;
    this->uv = uvs;
  }

  vec2 GetRealSize() {
    if (!tex) {
      return vec2();
    }
    return vec2(tex->width, tex->height);
  }
  vec2 GetSize() const { return size; }
  C3D_Tex* GetTex() const { return tex; };
  LI::Rect GetUV() const { return uv; }
  bool IsValid() const { return tex != 0; }

  bool AutoUnLoad() const { return autounload; }
  void AutoUnLoad(bool v) { autounload = v; }

  operator C3D_Tex*() const { return tex; }
  operator vec2() const { return size; }
  operator LI::Rect() const { return uv; }
  operator bool() const { return tex != 0; }

 private:
  void MakeTex(std::vector<u8>& buf, int w, int h, Type type = RGBA32,
               Filter filter = NEAREST);
  vec2 size;
  LI::Rect uv;
  C3D_Tex* tex = nullptr;
  bool autounload = true;
};
}  // namespace PD