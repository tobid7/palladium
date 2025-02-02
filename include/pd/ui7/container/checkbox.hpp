#pragma once

#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
class Checkbox : public Container {
 public:
  Checkbox(const std::string& label, vec2 pos, bool& usr_ref,
           LI::Renderer::Ref lr)
      : usr_ref(usr_ref) {
    this->screen = lr->CurrentScreen();
    this->label = label;
    this->SetPos(pos);
    this->tdim = lr->GetTextDimensions(label);
    color = UI7Color_FrameBackground;
    this->SetSize(cbs + vec2(tdim.x() + 5, 0));
  }
  ~Checkbox() {}

  void HandleInput(Hid::Ref inp) override;
  void Draw() override;

 private:
  vec2 tdim;
  vec2 cbs = vec2(18);
  UI7Color color;
  std::string label;
  bool& usr_ref;
};
}  // namespace UI7
}  // namespace PD