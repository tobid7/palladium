#include <pd/Texture.hpp>
#include <pd/external/stb_image.h>
#include <pd/internal_db.hpp>

#include <pd/external/stb_image_write.h>

namespace pdi {
static u32 GP2O(u32 v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return (v >= 64 ? v : 64);
}

static void R24R32(std::vector<uint8_t> &out,
                          const std::vector<uint8_t> &in, const int &w,
                          const int &h) {
  // Converts RGB24 to RGBA32
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int src = (y * w + x) * 3;
      int dst = (y * w + x) * 4;
      out[dst + 0] = in[src + 0];
      out[dst + 1] = in[src + 1];
      out[dst + 2] = in[src + 2];
      out[dst + 3] = 255;
    }
  }
}}

namespace Palladium {
void Texture::MakeTex(std::vector<unsigned char> &buf, int w, int h) {
  if (!tex) {
    _pdi_logger()->Write("Invalid Input (object has no adress!)");
    return;
  }
  // RGBA -> Abgr
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int pos = (x + y * w) * 4;
      auto r = buf[pos + 0];
      auto g = buf[pos + 1];
      auto b = buf[pos + 2];
      auto a = buf[pos + 3];
      buf[pos + 0] = a;
      buf[pos + 1] = b;
      buf[pos + 2] = g;
      buf[pos + 3] = r;
    }
  }

  // Pow2
  this->tex_size.x = pdi::GP2O((unsigned int)w);
  this->tex_size.y = pdi::GP2O((unsigned int)h);

  this->img_size.x = (u16)w;
  this->img_size.y = (u16)h;
  this->uvs.x = 0.0f;
  this->uvs.y = 1.0f;
  this->uvs.z = ((float)w / (float)this->tex_size.x);
  this->uvs.w = 1.0 - ((float)h / (float)this->tex_size.y);

  // Texture Setup
  C3D_TexInit(tex, (u16)this->tex_size.x, (u16)this->tex_size.y, GPU_RGBA8);
  C3D_TexSetFilter(tex, GPU_NEAREST, GPU_NEAREST);

  memset(tex->data, 0, tex->size);

  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int dst_pos = ((((y >> 3) * ((int)this->tex_size.x >> 3) + (x >> 3)) << 6) +
                     ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                      ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                    4;
      int src_pos = (y * w + x) * 4;

      memcpy(&((unsigned char *)tex->data)[dst_pos], &buf[src_pos], 4);
    }
  }

  C3D_TexFlush(tex);
  tex->border = 0x00000000;
  C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}
void Texture::LoadFile(const std::string& path) {
    int w, h, c = 0;
  unsigned char *image = stbi_load(path.c_str(), &w, &h, &c, 4);
  if (image == nullptr) {
    //_pdi_logger()->Write("Failed to Load Image: " + path);
    return;
  }
  // Size/Fmt Check
  if (w > 1024 || h > 1024) {
    // Reason: Image to Large
    //_pdi_logger()->Write("Image too Large!");
    stbi_image_free(image);
    return;
  }

  std::vector<unsigned char> wimg;
  if (c == 3) {
    //_pdi_logger()->Write("Convert Image to RGBA");
    stbi_image_free(image);
    image = stbi_load(path.c_str(), &w, &h, &c, 3);
    wimg.resize(w * h * 4);
    pdi::R24R32(wimg, std::vector<unsigned char>(image, image + (w * h * 3)),
                  w, h);
  } else {
    wimg.assign(&image[0], &image[(w * h * 4) - 1]);
    stbi_image_free(image);
  }
  // Create C2D_Image
  tex = new C3D_Tex;
  MakeTex(wimg, w, h);
}

void Texture::LoadFromMemory(const std::vector<unsigned char>& data) {
    int w, h, c = 0;
  unsigned char *image = stbi_load_from_memory(data.data(), data.size(), &w, &h, &c, 4);
  if (image == nullptr) {
    //_pdi_logger()->Write("Failed to Load Image: " + path);
    return;
  }
  // Size/Fmt Check
  if (w > 1024 || h > 1024) {
    // Reason: Image to Large
    //_pdi_logger()->Write("Image too Large!");
    stbi_image_free(image);
    return;
  }

  std::vector<unsigned char> wimg;
  if (c == 3) {
    //_pdi_logger()->Write("Convert Image to RGBA");
    stbi_image_free(image);
    image = stbi_load_from_memory(data.data(), data.size(), &w, &h, &c, 3);
    wimg.resize(w * h * 4);
    pdi::R24R32(wimg, std::vector<unsigned char>(image, image + (w * h * 3)),
                  w, h);
  } else {
    wimg.assign(&image[0], &image[(w * h * 4) - 1]);
    stbi_image_free(image);
  }
  // Create C2D_Image
  tex = new C3D_Tex;
  MakeTex(wimg, w, h);
}

void Texture::LoadPixels(const std::vector<unsigned char>& data, int w, int h) {
    Delete();
    if(w*h*4 != (int)data.size()) {
        return;
    }
    if (w > 1024 || h > 1024) {
    // Reason: Image to Large
    //_pdi_logger()->Write("Image too Large!");
    return;
  }
  tex = new C3D_Tex;
  std::vector<unsigned char> wimg(data);
  MakeTex(wimg, w, h);
}

void Texture::Delete() {
    if(tex) {
        delete tex;
        tex = nullptr;
        img_size = NVec2();
        tex_size = NVec2();
        uvs = NVec4();
    }
}
}