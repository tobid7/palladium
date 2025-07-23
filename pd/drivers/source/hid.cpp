#include <pd/drivers/hid.hpp>
#include <pd/drivers/pd_p_api.hpp>

namespace PD {
PD_DEF_EXP(HidDriver::Ref, Hid::pHid);

bool HidDriver::IsEvent(Event e, Key keys) { return KeyEvents[0][e] & keys; }
bool HidDriver::IsEvent(Event e, KbKey key) {
  if (!KbKeyEvents[0].count(key)) {
    return false;
  }
  return KbKeyEvents[0][key] == e;
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
}  // namespace PD