#pragma once

#include <3ds.h>
#include <citro2d.h>

#include <pd/NVec.hpp>
#include <pd/nimg.hpp>
#include <pd/smart_ctor.hpp>
#include <string>

namespace Palladium {
class Image {
 public:
  Image() = default;
  Image(C2D_Image img) { this->img = img; }
  Image(const std::string& path) { this->Load(path); }
  ~Image() = default;
  PD_SMART_CTOR(Image)
  void Load(const std::string& path);
  void From_NIMG(const nimg& image);
  void Delete();

  C2D_Image Get();
  C2D_Image& GetRef();
  void Set(const C2D_Image& i);
  NVec2 GetSize();
  bool Loadet();

 private:
  bool ext = false;
  C2D_Image img;
};
}  // namespace Palladium