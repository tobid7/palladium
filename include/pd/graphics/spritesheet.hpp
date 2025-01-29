#pragma once

#include <citro3d.h>
#include <tex3ds.h>

#include <pd/common/common.hpp>
#include <pd/graphics/lithium.hpp>
#include <pd/graphics/texture.hpp>

namespace PD {
class SpriteSheet {
 public:
  SpriteSheet() {}
  SpriteSheet(const std::string& path) { this->LoadFile(path); }
  ~SpriteSheet();

  void LoadFile(const std::string& path);
  Texture::Ref Get(int idx);
  int NumTextures() const;

  Texture::Ref operator[](int idx) { return Get(idx); }

 private:
  std::vector<Texture::Ref> textures;
};
}  // namespace PD