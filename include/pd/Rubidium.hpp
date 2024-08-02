#pragma once

#include <pd/nimg.hpp>
#include <pd/smart_ctor.hpp>

namespace Palladium {
class Rubidium {
 public:
  Rubidium(int w, int h);
  Rubidium();
  ~Rubidium();
  PD_SMART_CTOR(Rubidium)
  nimg& GetNimg() { return image; }
  void LoadFile(const std::string& path);
  void LoadNimg(const std::string& path);

  // Rendering
  void DrawPixel(int x, int y, unsigned int color);
  void DrawRect(int x, int y, int w, int h, unsigned int color, int t = 1);
  void DrawRectSolid(int x, int y, int w, int h, unsigned int color);
  void DrawLine(int x1, int y1, int x2, int y2, unsigned int color, int t = 1);
  void Flip(bool h, bool v);
  void EnableAA(bool enable) { enable_aa = enable; }

 private:
  // Leinwand (dont know english word for that)
  nimg image;
  bool enable_aa = true;
};
}  // namespace Palladium