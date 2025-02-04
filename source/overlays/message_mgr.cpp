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

#include <pd/overlays/message_mgr.hpp>

namespace PD {
MessageMgr::Container::Container(const std::string& title,
                                 const std::string& msg) {
  this->title = title;
  this->msg = msg;
  size = vec2(150, 50);
  // Precalculate colors
  col_bg = PD::Color("#111111aa");
  col_text = PD::Color("#ffffff");
  // Setup Flyin Animation
  FlyIn();
}
void MessageMgr::Container::Render(PD::LI::Renderer::Ref ren) {
  // Create a Temp var to not recalculate
  // the position everytime we use it
  // even if the calculation would always
  // result in the same we would waste a lot
  // of cpu performance which is a big issue
  // espeacilly on the Old3ds...
  vec2 tpos = pos;
  // Check if it goes out of screen
  // Instant kills cause it will never be on
  // Screen agains
  if (tpos[1] + size[1] < 0) {
    kill = true;
  }
  // If should be removed modify the color by fade
  // Use a temp var as well to not call it twice
  if (tbr) {
    float f = 1.f - pos.Progress();
    col_bg.a(col_bg.a() * f);
    col_text.a(col_text.a() * f);
  }
  // Create a backup Layer to Render
  // Text onto the next layer
  int l = ren->Layer();
  ren->DrawRectSolid(tpos, size, col_bg);
  ren->Layer(l + 1);
  ren->DrawText(tpos + vec2(4, 2), col_text, title);
  ren->DrawText(tpos + vec2(4, 16), col_text, msg);
  ren->Layer(l);
}
void MessageMgr::Container::Update(int slot, float delta) {
  // Increase lifetime
  lifetime += delta / 1000.f;
  // Trigger move up Animation if
  // the slot got changed
  if (s != slot) {
    ToBeMoved(slot);
    s = slot;
  }
  // Update the animations
  pos.Update(delta);
  // Trigger the remove Event if lifetime
  // goes beyond 4 secods
  if (lifetime > 4 && !tbr) {
    ToBeRemoved();
  }
}
void MessageMgr::Container::FlyIn() {
  // Come from out of the screen to 5, 185 into
  // The screen as EaseInSine in 0.5 seconds
  pos.From(vec2(-size[0], 240 - size[1] - 5))
      .To(vec2(5, 240 - size[1] - 5))
      .In(0.5)
      .As(pos.EaseInSine);
}
void MessageMgr::Container::ToBeMoved(int slot) {
  // Bit more special
  // Get the current pos as temp one
  vec2 tpos = pos;
  // Calculate a temp Startpos which
  // is the endpos of Flyin if
  // it is fully playd (see next comment)
  float spos = 240 - size[1] - 5;
  // Now from the Current Position Move up by
  // The Number of slot * the size.y + 5
  // and make sure it flys diagonal if the
  // Flyin hasn't ended yet. This animation uses EaseInSine
  // And does it's move in 0.4 seconds to not have
  // The new one and this one in collision for to much time
  pos.From(tpos)
      .To(vec2(5, spos - slot * (size[1] + 5)))
      .As(pos.EaseInSine)
      .In(0.4);
}

void MessageMgr::Container::ToBeRemoved() {
  // This effect uses EaseOutSine and as well uses the fade anim
  tbr = true;
  // We Force set the Position to Finished to avoid collision
  // to the ToBeMoved Animation And then set an EaseOutSine in 0.5
  // seconds to move the Message 30 pixels up while fading out
  pos.Finish();
  vec2 tpos = pos;
  pos.From(tpos).To(tpos - vec2(0, 30)).As(pos.EaseOutSine).In(0.5);
}

void MessageMgr::Push(const std::string& title, const std::string& text) {
  // Simply Add a New Message Container
  msgs.push_back(Container::New(title, text));
}

void MessageMgr::Update(float delta) {
  ren->OnScreen(ren->GetScreen(false));
  for (size_t i = 0; i < msgs.size(); i++) {
    // Update the Animation Handlers and Move older
    // Messages up if a new one got pushed
    msgs[i]->Update(msgs.size() - i - 1, delta);
    msgs[i]->Render(ren);
  }
  /// OMG HE LOOPS TWICE OVER THE OBJECTS TO
  /// CHECK IF THEY CAN BE REMOVED WOW.......
  /// Just my Stupid fix for the flickering
  /// Of the other messages if one get's removed
  for (size_t i = 0; i < msgs.size(); i++) {
    if (msgs[i]->ShouldBeRemoved()) {
      msgs.erase(msgs.begin() + i);
    }
  }
}
}  // namespace PD