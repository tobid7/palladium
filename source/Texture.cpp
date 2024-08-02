#include <pd/external/stb_image.h>
#include <pd/external/stb_image_write.h>

#include <pd/Texture.hpp>
#include <pd/internal_db.hpp>

namespace pdi {
static bool single_bit(unsigned int v) { return v && !(v & (v - 1)); }
static u32 get_pow2(u32 v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return (v >= 64 ? v : 64);
}

static void RGB24toRGBA32(std::vector<uint8_t> &out,
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
}
}  // namespace pdi

namespace Palladium {
GPU_TEXCOLOR GetTexFmt(Texture::Type type) {
  if (type == Texture::RGBA32)
    return GPU_RGBA8;
  else if (type == Texture::RGB24)
    return GPU_RGB8;
  else if (type == Texture::A8)
    return GPU_A8;
  return GPU_RGBA8;  // Default
}
int GetBPP(Texture::Type type) {
  if (type == Texture::RGBA32)
    return 4;
  else if (type == Texture::RGB24)
    return 3;
  else if (type == Texture::A8)
    return 1;
  return 0;  // Error
}
void Texture::MakeTex(std::vector<unsigned char> &buf, int w, int h,
                      Type type) {
  if (!tex) {
    _pdi_logger()->Write("Invalid Input (object has no adress!)");
    return;
  }
  // Don't check here as check done before
  int bpp = GetBPP(type);
  if (bpp == 4) {
    // RGBA -> Abgr
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        int pos = (x + y * w) * bpp;
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
  } else if (bpp == 3) {
    // RGBA -> Abgr
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        int pos = (x + y * w) * bpp;
        auto r = buf[pos + 0];
        auto g = buf[pos + 1];
        auto b = buf[pos + 2];
        buf[pos + 0] = b;
        buf[pos + 1] = g;
        buf[pos + 2] = r;
      }
    }
  }

  NVec2 tex_size(w, h);
  // Pow2
  if (!pdi::single_bit(w)) tex_size.x = pdi::get_pow2((unsigned int)w);
  if (!pdi::single_bit(h)) tex_size.y = pdi::get_pow2((unsigned int)h);

  this->img_size.x = (u16)w;
  this->img_size.y = (u16)h;
  this->uvs.x = 0.0f;
  this->uvs.y = 1.0f;
  this->uvs.z = ((float)w / (float)tex_size.x);
  this->uvs.w = 1.0 - ((float)h / (float)tex_size.y);

  // Texture Setup
  auto tex_fmt = GetTexFmt(type);
  C3D_TexInit(tex, (u16)tex_size.x, (u16)tex_size.y, tex_fmt);
  C3D_TexSetFilter(tex, GPU_NEAREST, GPU_NEAREST);

  memset(tex->data, 0, tex->size);

  if (bpp == 3 || bpp == 4) {
    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        int dst_pos = ((((y >> 3) * ((int)tex_size.x >> 3) + (x >> 3)) << 6) +
                       ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                        ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                      bpp;
        int src_pos = (y * w + x) * bpp;

        memcpy(&((unsigned char *)tex->data)[dst_pos], &buf[src_pos], bpp);
      }
    }
    C3D_TexFlush(tex);
  } else if (bpp == 1) {
    C3D_TexLoadImage(tex, buf.data(), GPU_TEXFACE_2D, 0);
  }

  tex->border = 0x00000000;
  C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}
void Texture::LoadFile(const std::string &path) {
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
    pdi::RGB24toRGBA32(
        wimg, std::vector<unsigned char>(image, image + (w * h * 3)), w, h);
  } else {
    wimg.assign(&image[0], &image[(w * h * 4) - 1]);
    stbi_image_free(image);
  }
  // Create C2D_Image
  tex = new C3D_Tex;
  MakeTex(wimg, w, h);
}

void Texture::LoadFromMemory(const std::vector<unsigned char> &data) {
  int w, h, c = 0;
  unsigned char *image =
      stbi_load_from_memory(data.data(), data.size(), &w, &h, &c, 4);
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
    pdi::RGB24toRGBA32(
        wimg, std::vector<unsigned char>(image, image + (w * h * 3)), w, h);
  } else {
    wimg.assign(&image[0], &image[(w * h * 4) - 1]);
    stbi_image_free(image);
  }
  // Create C2D_Image
  tex = new C3D_Tex;
  MakeTex(wimg, w, h);
}

NVec2 Texture::GetTexSize() {
  if (!tex) return NVec2();
  return NVec2(tex->width, tex->height);
}

void Texture::LoadPixels(const std::vector<unsigned char> &data, int w, int h,
                         Type type) {
  Delete();
  int bpp = GetBPP(type);
  Palladium::InlineAssert(bpp, "Invalid Type");
  if (w * h * bpp != (int)data.size()) {
    return;
  }
  if (w > 1024 || h > 1024) {
    // Reason: Image to Large
    //_pdi_logger()->Write("Image too Large!");
    return;
  }
  tex = new C3D_Tex;
  std::vector<unsigned char> wimg(data);
  MakeTex(wimg, w, h, type);
}

void Texture::ExternalLoad(C3D_Tex *tex, NVec2 rszs, NVec4 uvs) {
  Delete();
  this->tex = tex;
  this->img_size = rszs;
  this->uvs = uvs;
}

void Texture::Delete() {
  if (!ad) return;
  if (tex) {
    C3D_TexDelete(tex);
    delete tex;
    tex = nullptr;
    img_size = NVec2();
    this->uvs.x = 0.0f;
    this->uvs.y = 1.0f;
    this->uvs.z = 1.0f;
    this->uvs.w = 0.0f;
  }
}
}  // namespace Palladium