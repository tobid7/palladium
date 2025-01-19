#pragma once

#include <pd/graphics/lithium.hpp>
#include <pd/maths/color.hpp>
#include <pd/maths/tween.hpp>

namespace PD {
class MessageMgr : public PD::SmartCtor<MessageMgr> {
 public:
  class Container : public PD::SmartCtor<Container> {
   public:
    Container(const std::string& title, const std::string& msg);
    ~Container() {}

    void Render(PD::LI::Renderer::Ref ren);
    void Update(int slot, float delta);
    void FlyIn();
    void ToBeMoved(int slot);
    void ToBeRemoved();

    bool ShouldBeRemoved() const { return (tbr && pos.IsFinished()) || kill; }

   private:
    PD::Color col_bg;       // Background Color
    PD::Color col_text;     // Text Color
    float lifetime = 0.f;   // LifeTime
    PD::Tween<vec2> pos;    // Position effect
    std::string title;      // Title
    std::string msg;        // Message
    vec2 size;              // Size of the Background
    bool tbr = false;       // To be Removed ?
    bool kill = false;      // Instant Kill
    int s = 0;              // Slot
  };
  MessageMgr(PD::LI::Renderer::Ref r) { ren = r; }
  ~MessageMgr() {}

  void Push(const std::string& title, const std::string& text);
  void Update(float delta);

 private:
  std::vector<Container::Ref> msgs;
  PD::LI::Renderer::Ref ren;
};
}  // namespace PD