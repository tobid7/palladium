#include <pd/Sheet.hpp>
#include <pd/internal_db.hpp>

Result Palladium::Sheet::Load(const std::string& path) {
  if (this->spritesheet) Free();
  this->spritesheet = C2D_SpriteSheetLoad(path.c_str());
  if (!this->spritesheet) {
    _pdi_logger()->Write("Failed to Load Spritesheet from: " + path, 0);
  }
  return 0;
}

void Palladium::Sheet::Free() {
  if (!this->spritesheet) return;
  C2D_SpriteSheetFree(this->spritesheet);
  this->spritesheet = nullptr;
}

C2D_Image Palladium::Sheet::GetImage(int idx) {
  if (!this->spritesheet) return {nullptr, nullptr};
  return C2D_SpriteSheetGetImage(this->spritesheet, idx);
}