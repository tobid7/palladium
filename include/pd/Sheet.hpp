#pragma once
#include <tex3ds.h>

#include <pd/Image.hpp>
#include <pd/Texture.hpp>
#include <pd/smart_ctor.hpp>

namespace Palladium {
class Sheet {
 public:
  Sheet() = default;
  ~Sheet() = default;
  PD_SMART_CTOR(Sheet)
  void LoadT3X(const std::string& path);
  Texture::Ref Get(int idx);
  Image::Ref GetImage(int idx);

 private:
  std::vector<Texture::Ref> sprites;
  Tex3DS_Texture sheet;
  C3D_Tex* sheet_tex = nullptr;
};
}  // namespace Palladium
