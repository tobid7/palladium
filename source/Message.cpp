#include <algorithm>
#include <memory>
#include <pd/Color.hpp>
#include <pd/Message.hpp>
#include <pd/palladium.hpp>
#include <vector>

extern bool pdi_debugging;

static std::vector<std::shared_ptr<Palladium::Message>> msg_lst;
static int fade_outs = 200;  // Start of fadeout
static int idles = 60;       // start of Idle
static int anim_len = 300;   // Full Length of Animation

NVec2 MakePos(int frame, int entry) {
  float fol = anim_len - fade_outs;
  if (frame > fade_outs)
    return NVec2(5, 240 - ((entry + 1) * 55) - 5 +
                         (float)((frame - fade_outs) / fol) * -20);
  if (frame > idles) return NVec2(5, 240 - ((entry + 1) * 55) - 5);
  return NVec2(-150 + ((float)(frame / (float)idles) * 155),
                240 - ((entry + 1) * 55) - 5);
}

namespace Palladium {
float GetDeltaTime();  // Extern from Palladium.cpp

void ProcessMessages() {
  float tmp_txt = R2::GetTextSize();
  R2::DefaultTextSize();
  // Draw in ovl mode
  R2::OnScreen(R2Screen_Top);
  float fol = anim_len - fade_outs;
  std::reverse(msg_lst.begin(), msg_lst.end());
  for (size_t i = 0; i < msg_lst.size(); i++) {
    NVec2 pos = MakePos(msg_lst[i]->animationframe, i);
    if ((pos.y + 150) < 0) {
      // Dont Render Out of Screen
      // And as thay aren't relevant anymore
      // Thay get deleted!
      msg_lst.erase(msg_lst.begin() + i);
    } else {
      int new_alpha = 200;
      if (msg_lst[i]->animationframe > fade_outs) {
        new_alpha =
            200 - (float(msg_lst[i]->animationframe - fade_outs) / fol) * 200;
      }
      // Wtf is this function lol
      auto bgc = Palladium::Color::RGBA(PDColor_MessageBackground)
                     .changeA(new_alpha)
                     .toRGBA();
      auto tc =
          Palladium::Color::RGBA(PDColor_Text2).changeA(new_alpha).toRGBA();
      R2::AddRect(pos, NVec2(150, 50), bgc);
      R2::AddText(pos + NVec2(5, 1), msg_lst[i]->title, tc);
      R2::AddText(pos + NVec2(5, 17), msg_lst[i]->message, tc);
      if (pdi_debugging)
        R2::AddText(pos + NVec2(155, 1),
                      std::to_string(msg_lst[i]->animationframe), tc);
      // Why Frameadd? because Message uses int as frame and
      // It seems that lower 0.5 will be rounded to 0
      // Why not replace int with float ?
      // cause of buggy positions (seen in Flappy Bird 3ds for example)
      float frameadd = 60.f * Palladium::GetDeltaTime();
      // 60fps animation * delta to not slowdown
      // Oh and fix for Startup lol
      // Todo: Only do this on AppStart
      if (msg_lst[i]->animationframe == 0) {
        msg_lst[i]->animationframe += 1;
      } else {
        msg_lst[i]->animationframe += (frameadd < 1.f ? 1.f : frameadd);
      }
      if (msg_lst[i]->animationframe > anim_len) {
        msg_lst.erase(msg_lst.begin() + i);
      }
    }
  }
  // ReReverse ?? lol
  // Cause otherwise the Toasts will swap
  std::reverse(msg_lst.begin(), msg_lst.end());
  R2::SetTextSize(tmp_txt);
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