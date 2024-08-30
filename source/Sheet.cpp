#include <3ds.h>

#include <fstream>
#include <pd/Sheet.hpp>

namespace Palladium {
void Sheet::LoadT3X(const std::string& path) {
  if (sheet_tex) {
    C3D_TexDelete(sheet_tex);
    delete sheet_tex;
    sheet_tex = nullptr;
  }
  sheet_tex = new C3D_Tex;
  std::fstream f(path, std::ios::in | std::ios::binary);
  if (!f) {
    delete sheet_tex;
    sheet_tex = nullptr;
    return;
  }
  f.seekg(0, std::ios::end);
  std::vector<unsigned char> dat(f.tellg());
  f.seekg(0, std::ios::beg);
  f.read(reinterpret_cast<char*>(dat.data()), dat.size());
  sheet =
      Tex3DS_TextureImport(dat.data(), dat.size(), sheet_tex, nullptr, true);
  C3D_TexSetFilter(sheet_tex, GPU_LINEAR, GPU_LINEAR);
  f.close();
  int st = Tex3DS_GetNumSubTextures(sheet);
  sprites.resize(st);
  for (int i = 0; i < st; i++) {
    sprites[i] = Texture::New();
    auto stex = Tex3DS_GetSubTexture(sheet, i);
    sprites[i]->ExternalLoad(
        sheet_tex, NVec2(stex->width, stex->height),
        NVec4(stex->left, stex->top, stex->right, stex->bottom));
    sprites[i]->AutoDelete(false);
  }
}

Texture::Ref Sheet::Get(int idx) {
  if (idx < 0 || idx >= (int)sprites.size()) return nullptr;
  return sprites[idx];
}

Image::Ref Sheet::GetImage(int idx) {
  if (idx < 0 || idx >= (int)sprites.size()) return nullptr;
  Image::Ref img = Image::New();
  img->Set(sprites[idx], sprites[idx]->GetUV());
  return img;
}
}  // namespace Palladium