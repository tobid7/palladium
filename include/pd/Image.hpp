#pragma once

#include <3ds.h>

#include <pd/maths/NVec.hpp>
#include <pd/Texture.hpp>
#include <pd/nimg.hpp>
#include <pd/smart_ctor.hpp>
#include <string>

namespace Palladium {
class Image {
 public:
  Image() = default;
  Image(const std::string& path) { this->Load(path); }
  ~Image() = default;
  PD_SMART_CTOR(Image)
  void Load(const std::string& path);
  void From_NIMG(const nimg& image);
  void Delete();

  Texture::Ref Get();
  void Set(Texture::Ref i, NVec4 uvs = NVec4(-1, -1, -1, -1));
  NVec2 GetSize();
  NVec4 GetUV() {
    return (custom_uvs.x() != -1) ? custom_uvs : img->GetUV();
  }
  bool Loadet();

 private:
  bool ext = false;
  Texture::Ref img;
  NVec4 custom_uvs = NVec4(-1, -1, -1, -1);
};
}  // namespace Palladium