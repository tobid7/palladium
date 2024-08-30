#pragma once

#include <string>

namespace Palladium {
struct Message {
  Message(std::string t, std::string m) {
    title = t;
    message = m;
    animtime = 0.f;
  }

  std::string title;
  std::string message;
  float animtime;
};

void ProcessMessages();
void PushMessage(const Message& msg);
inline void PushMessage(const std::string& head, const std::string& msg) {
  PushMessage(Message(head, msg));
}
// Config
void SetMessageIdleStartFrame(int frame);
void SetMessageTotalAnimationFrames(int total_frames);
void SetMessageFadeOutStartFrame(int frame);
}  // namespace Palladium