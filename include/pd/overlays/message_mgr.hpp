#pragma once

/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <pd/lithium/renderer.hpp>
#include <pd/core/color.hpp>
#include <pd/core/tween.hpp>

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
    PD::Color col_bg;      // Background Color
    PD::Color col_text;    // Text Color
    float lifetime = 0.f;  // LifeTime
    PD::Tween<vec2> pos;   // Position effect
    std::string title;     // Title
    std::string msg;       // Message
    vec2 size;             // Size of the Background
    bool tbr = false;      // To be Removed ?
    bool kill = false;     // Instant Kill
    int s = 0;             // Slot
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