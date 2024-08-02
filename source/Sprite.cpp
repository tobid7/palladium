#include <pd/Sprite.hpp>

void Palladium::Sprite::FromSheet(Palladium::Sheet::Ref sheet, size_t index) {
  C2D_SpriteFromSheet(&this->sprite, sheet->Get(), index);
}
bool Palladium::Sprite::Draw() {
  // Patch Depth before draw
  sprite.params.depth = 0.5;
  return C2D_DrawSprite(&this->sprite);
}
void Palladium::Sprite::SetCenter(float x, float y) {
  C2D_SpriteSetCenter(&this->sprite, x, y);
}
void Palladium::Sprite::SetPos(float x, float y) {
  C2D_SpriteSetPos(&this->sprite, x, y);
}
void Palladium::Sprite::SetRotation(float rotation) {
  C2D_SpriteSetRotation(&this->sprite, rotation);
}
void Palladium::Sprite::Rotate(float speed) {
  C2D_SpriteRotateDegrees(&this->sprite, speed);
}
float Palladium::Sprite::GetHeight() { return GetSize().x; }
float Palladium::Sprite::GetWidth() { return GetSize().y; }
float Palladium::Sprite::GetPosX() { return GetPos().x; }
float Palladium::Sprite::GetPosY() { return GetPos().y; }

NVec2 Palladium::Sprite::GetPos() {
  return NVec2(this->sprite.params.pos.x, this->sprite.params.pos.y);
}

NVec2 Palladium::Sprite::GetSize() {
  return NVec2(this->sprite.params.pos.w, this->sprite.params.pos.h);
}

void Palladium::Sprite::SetPos(NVec2 pos) {
  C2D_SpriteSetPos(&this->sprite, pos.x, pos.y);
}

void Palladium::Sprite::SetScale(NVec2 scale) {
  C2D_SpriteScale(&this->sprite, scale.x, scale.y);
}
void Palladium::Sprite::SetRotCenter(NVec2 percentage) {
  C2D_SpriteSetCenter(&this->sprite, percentage.x, percentage.y);
}

void Palladium::Sprite::FromImage(Palladium::Image::Ref img) {
  // C2D_SpriteFromImage(&this->sprite, img->Get());
}

void Palladium::Sprite::SetScale(float x, float y) {
  C2D_SpriteScale(&this->sprite, x, y);
}