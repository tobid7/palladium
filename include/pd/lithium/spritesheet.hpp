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

#include <citro3d.h>
#include <tex3ds.h>

#include <pd/core/common.hpp>
#include <pd/lithium/texture.hpp>

namespace PD {
/**
 * Tex 3DS Spritesheet integration to Lithium
 */
class SpriteSheet : public SmartCtor<SpriteSheet> {
 public:
  SpriteSheet() = default;
  /**
   * Constructor to directly load a spritesheet
   * @param path Path to spritesheet
   */
  SpriteSheet(const std::string& path) { this->LoadFile(path); }
  /**
   * Deconstructor to unload the Spritesheet
   */
  ~SpriteSheet();

  /**
   * Function to load a Spritesheet
   * @param path Path to the file
   */
  void LoadFile(const std::string& path);
  /** Get a Textures Reference */
  Texture::Ref Get(int idx);
  /** Get Number of Textures in spritesheet */
  int NumTextures() const;

  /** Operator to get Texture reference */
  Texture::Ref operator[](int idx) { return Get(idx); }

 private:
  /** Storage of the Spritesheets textures */
  std::vector<Texture::Ref> textures;
};
}  // namespace PD