// WARNING
// THIS IS BETA STUFF
// ITS MAKE LIKE EXTERNAL BUT
// FOR Palladium ITS INTEGRATED

#pragma once

#include <pd/NVec.hpp>
#include <string>

namespace Palladium {
namespace Hid {
enum Actions {
  Down = 0,
  Held = 1,
  Up = 2,
  DownRepeat = 3,
};
// Register Functions
// Register Current state values
void RegKeyDown(uint32_t &key_down);
void RegKeyHeld(uint32_t &key_held);
void RegKeyUp(uint32_t &key_up);
void RegKeyRepeat(uint32_t &repeat);
void RegTouchCoords(NVec2 &touch_pos);
// Not Corectly Implemented Yet
void RegAnalog1Movement(NVec2 &movement);
void RegAnalog2Movement(NVec2 &movement);
// Register Keys
void RegKeyEvent(const std::string &event, uint32_t key);
// KeyEvents
bool IsEvent(const std::string &event, Actions action);
NVec2 GetTouchPosition();
NVec2 GetLastTouchPosition();
NVec2 GetTouchDownPosition();
void Update();
// Lock/Unlock Input api for example for Keyboard
void Lock();
void Unlock();
void Clear();
}  // namespace Hid
}  // namespace Palladium