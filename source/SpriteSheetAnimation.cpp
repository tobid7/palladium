#include <pd/SpriteAnimation.hpp>

void Palladium::SpriteSheetAnimation::Setup(Palladium::Sheet::Ref sheet,
                                            size_t imagecount,
                                            size_t startimage,
                                            float frame_begin,
                                            float frame_finish) {
  D_totaltime = frame_begin;

  this->images = imagecount;

  this->sheet = sheet;

  this->time = frame_finish;

  Palladium::SpriteSheetAnimation::FromSheet(this->sheet, startimage);
}
void Palladium::SpriteSheetAnimation::Play(float timespeed) {
  D_totaltime += timespeed;
  if (D_totaltime >= time) {
    D_totaltime -= time;
    imgs++;
    if (imgs == images) {
      imgs = 0;
    }
  }
  Palladium::SpriteSheetAnimation::FromSheet(sheet, imgs);
  // Palladium::SpriteSheetAnimation::Draw();
}