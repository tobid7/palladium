#pragma once

#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
class Button : public Container {
 public:
  Button(const std::string& label, vec2 pos, LI::Renderer::Ref lr) {
    this->screen = lr->CurrentScreen();
    this->label = label;
    this->SetPos(pos);
    this->tdim = lr->GetTextDimensions(label);
    color = UI7Color_Button;
    this->SetSize(tdim + vec2(8, 4));
  }
  ~Button() {}

  bool IsPressed() { return pressed; }
  void HandleInput(Hid::Ref inp) override;
  void Draw() override;

 private:
  vec2 tdim;
  UI7Color color;
  std::string label;
  bool pressed = false;
};
}  // namespace UI7
}  // namespace PD