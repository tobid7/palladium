#include <pd/external/stb_image.h>

#include <pd/Image.hpp>
#include <pd/internal_db.hpp>
#include <vector>

namespace Palladium {

void Image::Load(const std::string &path) {
  // Make sure to cleanup
  Delete();
  if (!img) img = Texture::New();
  img->LoadFile(path);
}

void Image::From_NIMG(const nimg &image) {
  // Make sure to cleanup
  Delete();
  if (!img) img = Texture::New();
  img->LoadPixels(image.pixel_buffer, image.width, image.height);
}

Texture::Ref Image::Get() {
  if (!Loadet()) {
    _pdi_logger()->Write("Image not Loadet!");
  }
  return img;
}

void Image::Set(Texture::Ref i, NVec4 uvs) {
  Delete();
  if(uvs.x != -1) custom_uvs = uvs;
  img = i;
}

NVec2 Image::GetSize() {
  if (!img) return NVec2(0, 0);
  return img->GetSize();
}

void Image::Delete() { img = nullptr; }

bool Image::Loadet() { return img != nullptr; }
}  // namespace Palladium