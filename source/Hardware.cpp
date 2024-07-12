#include <pd/Hardware.hpp>
#include <pd/internal_db.hpp>

// Os Specific includes
#include <3ds.h>

void Palladium::Hardware::Initialisize() {
  mcuHwcInit();
  atexit(mcuHwcExit);
  ptmuInit();
  atexit(ptmuExit);
}

bool Palladium::Hardware::IsHeadphones() {
  if (pdi_is_ndsp) {
    bool inserted;
    DSP_GetHeadphoneStatus(&inserted);
    return inserted;
  } else
    return false;
}

bool Palladium::Hardware::IsCharging() {
  uint8_t var;
  PTMU_GetBatteryChargeState(&var);
  // Some Security Stuff
  if (var < 0x00 && var > 0x01) {
    return false;
  }
  return (var == 0x01 ? true : false);
}

int Palladium::Hardware::GetBatteryPercentage() {
  uint8_t percentLevel = 0;
  MCUHWC_GetBatteryLevel(&percentLevel);
  return percentLevel;
}

float Palladium::Hardware::Get3dSliderLevel() { return osGet3DSliderState(); }

float Palladium::Hardware::GetSoundSliderLevel() {
  uint8_t percentLevel;
  MCUHWC_GetSoundSliderLevel(&percentLevel);
  return (float)percentLevel / 100;
}

int Palladium::Hardware::GetWifiLevel() { return osGetWifiStrength(); }