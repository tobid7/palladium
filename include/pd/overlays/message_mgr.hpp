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

#include <pd/core/core.hpp>
#include <pd/lithium/renderer.hpp>

namespace PD {
/**
 * Message Manager
 */
class MessageMgr : public PD::SmartCtor<MessageMgr> {
 public:
  /**
   * Message Container
   */
  class Container : public PD::SmartCtor<Container> {
   public:
    /**
     * Message Constructor
     * @param title Title
     * @param msg Message
     */
    Container(const std::string& title, const std::string& msg);
    ~Container() = default;

    /** Render the Container */
    void Render(PD::LI::Renderer::Ref ren);
    /**
     * Update Animations by slot and delta
     * @param slot Slot
     * @param delta Deltatime
     */
    void Update(int slot, float delta);
    /** Trigger Fly in Animation */
    void FlyIn();
    /** Trigger Change Position Animation */
    void ToBeMoved(int slot);
    /** Trigger Removed Animation */
    void ToBeRemoved();

    /** Check if Message can be removed from list */
    bool ShouldBeRemoved() const { return (tbr && pos.IsFinished()) || kill; }

   private:
    PD::Color col_bg;      // Background Color
    PD::Color col_text;    // Text Color
    float lifetime = 0.f;  // LifeTime
    PD::Tween<fvec2> pos;  // Position effect
    std::string title;     // Title
    std::string msg;       // Message
    fvec2 size;            // Size of the Background
    bool tbr = false;      // To be Removed ?
    bool kill = false;     // Instant Kill
    int s = 0;             // Slot
  };
  /** Constructor to Link a Renderer reference */
  MessageMgr(PD::LI::Renderer::Ref r) { ren = r; }
  ~MessageMgr() = default;

  /**
   * Add a New Message
   * @param title Message Title
   * @param text Message Text
   */
  void Push(const std::string& title, const std::string& text);
  /**
   * Update Messages
   * @param delta Deltatime
   */
  void Update(float delta);

 private:
  std::vector<Container::Ref> msgs;  // List of Messages
  PD::LI::Renderer::Ref ren;         // Renderer Reference
};
}  // namespace PD