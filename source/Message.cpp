#include <algorithm>
#include <memory>
#include <pd/base/Color.hpp>
#include <pd/Message.hpp>
#include <pd/palladium.hpp>
#include <pd/Lithium.hpp>
#include <vector>

extern bool pdi_debugging;

static std::vector<std::shared_ptr<Palladium::Message>> msg_lst;
static int fade_outs = 200;             // Start of fadeout
static int idles = 60;                  // start of Idle
static int anim_len = 300;              // Full Length of Animation
static NVec2 msg_box = NVec2(170, 50);  // Message Box Size

NVec2 MakePos(float anim_time, int entry) {
  float fol = anim_len - fade_outs;
  if (anim_time > fade_outs)
    return NVec2(
        5, static_cast<int>(240 - ((entry + 1) * 55) - 5 +
                            (float)((anim_time - fade_outs) / fol) * -20));
  if (anim_time > idles) return NVec2(5, 240 - ((entry + 1) * 55) - 5);
  return NVec2(
      static_cast<int>(-150 + ((float)(anim_time / (float)idles) * 155)),
      240 - ((entry + 1) * 55) - 5);
}

namespace Palladium {
float GetDeltaTime();  // Extern from Palladium.cpp

void ProcessMessages() {
  float tmp_txt = LI::GetTextScale();
  LI::DefaultTextScale();
  // Draw in ovl mode
  LI::OnScreen(false);
  LI::NewLayer();
  float fol = anim_len - fade_outs;
  std::reverse(msg_lst.begin(), msg_lst.end());
  for (size_t i = 0; i < msg_lst.size(); i++) {
    NVec2 pos = MakePos(msg_lst[i]->animtime, i);
    if ((pos.y() + 150) < 0) {
      // Dont Render Out of Screen
      // And as thay aren't relevant anymore
      // Thay get deleted!
      msg_lst.erase(msg_lst.begin() + i);
    } else {
      int new_alpha = 200;
      if (msg_lst[i]->animtime > fade_outs) {
        new_alpha = 200 - (float(msg_lst[i]->animtime - fade_outs) / fol) * 200;
      }
      // Wtf is this function lol
      auto bgc = Palladium::Color::RGBA(PDColor_MessageBackground)
                     .changeA(new_alpha)
                     .toRGBA();
      auto tc =
          Palladium::Color::RGBA(PDColor_Text2).changeA(new_alpha).toRGBA();
      LI::DrawRect(pos, msg_box, bgc);
      LI::NewLayer();
      LI::DrawText(pos + NVec2(5, 1), tc, msg_lst[i]->title);
      LI::DrawText(pos + NVec2(5, 17), tc, msg_lst[i]->message);
      if (pdi_debugging)
        LI::DrawText(pos + NVec2(msg_box.x() + 5, 1), tc,
                    std::to_string((int)msg_lst[i]->animtime));
      // fix for Startup lol
      // Todo: Only do this on AppStart
      if (msg_lst[i]->animtime == 0) {
        msg_lst[i]->animtime += 1;
      } else {
        msg_lst[i]->animtime += Palladium::GetDeltaTime()*0.1f;
      }
      if (msg_lst[i]->animtime > anim_len) {
        msg_lst.erase(msg_lst.begin() + i);
      }
    }
  }
  // ReReverse ?? lol
  // Cause otherwise the Toasts will swap
  std::reverse(msg_lst.begin(), msg_lst.end());
  LI::SetTextScale(tmp_txt);
}

void PushMessage(const Message &msg) {
  msg_lst.push_back(std::make_shared<Palladium::Message>(msg));
}

void SetMessageIdleStartFrame(int frame) { idles = frame; }

void SetMessageTotalAnimationFrames(int total_frames) {
  anim_len = total_frames;
}

void SetMessageFadeOutStartFrame(int frame) { fade_outs = frame; }
}  // namespace Palladium