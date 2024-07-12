#pragma once

#include <citro2d.h>

#include <fstream>
#include <memory>
#include <pd/Error.hpp>
#include <pd/smart_ctor.hpp>

namespace Palladium {
class Font {
 public:
  Font() = default;
  Font(const std::string& path) { Load(path); };
  ~Font() { Unload(); }
  PD_SMART_CTOR(Font)

  void Load(const std::string& path) {
    std::ifstream ft(path, std::ios::in | std::ios::binary);
    bool io = ft.is_open();
    ft.close();
    Palladium::InlineAssert(io, "File not Found!");
    fnt = C2D_FontLoad(path.c_str());
    Palladium::InlineAssert(fnt, "Font could not be loaded!");
  }
  C2D_Font Ptr() { return fnt; }
  void Unload() {
    if (!fnt) return;
    C2D_FontFree(fnt);
    fnt = nullptr;
  }

 private:
  C2D_Font fnt = nullptr;
};
}  // namespace Palladium
