#include <pd/drivers/hid.hpp>
#include <pd/drivers/pd_p_api.hpp>

namespace PD {
PD_DEF_EXP(HidDriver::Ref, Hid::pHid);

bool HidDriver::IsEvent(Event e, Key keys) { return KeyEvents[0][e] & keys; }
bool HidDriver::IsEvent(Event e, KbKey keys) {
  return KbKeyEvents[0][e].Has(keys);
}

void HidDriver::SwapTab() {
  auto tkd = KeyEvents[1][Event_Down];
  auto tkh = KeyEvents[1][Event_Held];
  auto tku = KeyEvents[1][Event_Up];
  KeyEvents[1][Event_Down] = KeyEvents[0][Event_Down];
  KeyEvents[1][Event_Held] = KeyEvents[0][Event_Held];
  KeyEvents[1][Event_Up] = KeyEvents[0][Event_Up];
  KeyEvents[0][Event_Down] = tkd;
  KeyEvents[0][Event_Held] = tkh;
  KeyEvents[0][Event_Up] = tku;
}

/**
 * If this func has no verride, still clear the stats
 * cause if they are empty this leads to a crash
 */
void HidDriver::Update() {
  // Clear States
  for (int i = 0; i < 2; i++) {
    KeyEvents[i][Event_Down] = 0;
    KeyEvents[i][Event_Held] = 0;
    KeyEvents[i][Event_Up] = 0;
    for (auto& it : KbKeyEvents[i]) {
      it.second = Event_Null;
    }
  }
}
}  // namespace PD