#include <pd/drivers/hid.hpp>

namespace PD {
PD_API std::unique_ptr<HidDriver> Hid::driver;

PD_API HidDriver::HidDriver(std::string_view name) : DriverInterface(name) {}

PD_API HidDriver::~HidDriver() {}

PD_API bool HidDriver::IsEvent(Event e, HidInternal::GamepadKey keys) {
  return pGamepadEvents[0][e] & keys;
}

PD_API bool HidDriver::IsEvent(Event e, HidInternal::Keyboard::Key keys) {
  return pKeyboardEvents[0][e].Has(keys);
}
/**
 * Todo: Keyboard support
 */
PD_API void HidDriver::SwapTab() {
  auto tkd = pGamepadEvents[1][Event::Down];
  auto tkh = pGamepadEvents[1][Event::Held];
  auto tku = pGamepadEvents[1][Event::Up];
  pGamepadEvents[1][Event::Down] = pGamepadEvents[0][Event::Down];
  pGamepadEvents[1][Event::Held] = pGamepadEvents[0][Event::Held];
  pGamepadEvents[1][Event::Up] = pGamepadEvents[0][Event::Up];
  pGamepadEvents[0][Event::Down] = tkd;
  pGamepadEvents[0][Event::Held] = tkh;
  pGamepadEvents[0][Event::Up] = tku;
}

/**
 * If this func has no verride, still clear the stats
 * cause if they are empty this leads to a crash
 */
PD_API void HidDriver::Update() {
  // Clear States
  for (int i = 0; i < 2; i++) {
    pGamepadEvents[i][Event::Down] = 0;
    pGamepadEvents[i][Event::Held] = 0;
    pGamepadEvents[i][Event::Up] = 0;
    for (auto& it : pKeyboardEvents[i]) {
      it.second = 0;  // ? why was this Event_Null
    }
  }
}
}  // namespace PD