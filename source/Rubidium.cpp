#include <pd/external/stb_image.h>

#include <pd/Color.hpp>
#include <pd/Rubidium.hpp>

void d7_pixel_blend(Palladium::Rubidium* rb, int x, int y, unsigned int clr,
                    float blend) {
  Palladium::Color::RGBA cl(clr);
  cl.fade_to(Palladium::Color::RGBA(0.f, 0.f, 0.f, 1.f), blend);
  rb->DrawPixel(x, y, cl.toRGBA());
}

namespace Palladium {
Rubidium::Rubidium(int w, int h) { image = Palladium::nimg(w, h); }
Rubidium::Rubidium() { image = Palladium::nimg(1, 1); }

Rubidium::~Rubidium() {
  // Do nothing
}

void Rubidium::LoadFile(const std::string& path) {
  int w, h, c;
  uint8_t* dat = stbi_load(path.c_str(), &w, &h, &c, 4);
  image = nimg(w, h);
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int pos = (y * w + x) * 4;
      image.pixel_buffer[pos + 0] = dat[pos + 0];
      image.pixel_buffer[pos + 1] = dat[pos + 1];
      image.pixel_buffer[pos + 2] = dat[pos + 2];
      image.pixel_buffer[pos + 3] = dat[pos + 3];
    }
  }
  stbi_image_free(dat);
}

void Rubidium::LoadNimg(const std::string& path) {
  image = Palladium::NIMG_Load(path);
}

void Rubidium::DrawPixel(int x, int y, unsigned int color) {
  if (x > image.width || x < 0 || y > image.height || y < 0) return;
  Palladium::Color::RGBA splitter(color);
  image.pixel_buffer[((y * image.width + x) * 4) + 0] = splitter.m_r;
  image.pixel_buffer[((y * image.width + x) * 4) + 1] = splitter.m_g;
  image.pixel_buffer[((y * image.width + x) * 4) + 2] = splitter.m_b;
  image.pixel_buffer[((y * image.width + x) * 4) + 3] = splitter.m_a;
}

void Rubidium::DrawRect(int x, int y, int w, int h, unsigned int color, int t) {
  DrawLine(x, y, x + w, y, color, t);
  DrawLine(x, y, x, y + h, color, t);
  DrawLine(x, y + h, x + w, y + h, color, t);
  DrawLine(x + w, y, x + w, y + h, color, t);
}

void Rubidium::DrawRectSolid(int x, int y, int w, int h, unsigned int color) {
  for (int ix = x; ix < x + w; ix++) {
    for (int iy = y; iy < y + h; iy++) {
      DrawPixel(ix, iy, color);
    }
  }
}

void Rubidium::DrawLine(int x1, int y1, int x2, int y2, unsigned int color,
                        int t) {
  // Reference
  // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;
  int ht = t / 2;
  while (true) {
    for (int i = -ht; i <= ht; i++) {
      if (dy <= dx) {
        DrawPixel(x1, y1 + i, color);
      } else {
        DrawPixel(x1 + i, y1, color);
      }
    }
    if (x1 == x2 && y1 == y2) break;
    int e2 = err * 2;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}

void Rubidium::Flip(bool h, bool v) {
  const nimg _bak = image;
  if (h) {
    for (int x = 0; x < image.width; x++) {
      for (int y = 0; y < image.height; y++) {
        int src = (y * image.width + x) * 4;
        int dst = ((image.height - 1 - y) * image.width + x) * 4;
        image.pixel_buffer[src + 0] = _bak.pixel_buffer[dst + 0];
        image.pixel_buffer[src + 1] = _bak.pixel_buffer[dst + 1];
        image.pixel_buffer[src + 2] = _bak.pixel_buffer[dst + 2];
        image.pixel_buffer[src + 3] = _bak.pixel_buffer[dst + 3];
      }
    }
  }

  if (v) {
    for (int x = 0; x < image.width; x++) {
      for (int y = 0; y < image.height; y++) {
        int src = (y * image.width + x) * 4;
        int dst = (y * image.width + (image.width - 1 - x)) * 4;
        image.pixel_buffer[src + 0] = _bak.pixel_buffer[dst + 0];
        image.pixel_buffer[src + 1] = _bak.pixel_buffer[dst + 1];
        image.pixel_buffer[src + 2] = _bak.pixel_buffer[dst + 2];
        image.pixel_buffer[src + 3] = _bak.pixel_buffer[dst + 3];
      }
    }
  }
}
}  // namespace Palladium