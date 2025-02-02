#pragma once

#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
class Label : public Container {
 public:
  Label(const std::string& label, vec2 pos, LI::Renderer::Ref lr) {
    this->screen = lr->CurrentScreen();
    this->label = label;
    this->SetPos(pos);
    this->tdim = lr->GetTextDimensions(label);
    color = UI7Color_Text;
    this->SetSize(tdim);
  }
  ~Label() {}

  void Draw() override;

 private:
  vec2 tdim;
  UI7Color color;
  std::string label;
};
}  // namespace UI7
}  // namespace PD