#pragma once

#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
class Image : public Container {
 public:
  Image(Texture::Ref img, vec2 pos, LI::Renderer::Ref lr, vec2 size = 0.f) {
    this->screen = lr->CurrentScreen();
    this->img = img;
    this->SetPos(pos);
    if (size.x() != 0 || size.y() != 0) {
      this->SetSize(size);
    } else {
      this->SetSize(img->GetSize());
    }
  }
  ~Image() {}

  void Draw() override;

 private:
  Texture::Ref img;
};
}  // namespace UI7
}  // namespace PD