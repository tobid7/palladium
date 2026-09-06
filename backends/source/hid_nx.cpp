
#include <pd_system/hid_nx.hpp>

#ifdef PD_ENABLE_HID_NX
#include <switch.h>

namespace PD {
struct HidNX::Impl {
  PadState Pad;
};

HidNX::HidNX() : HidDriver("HidNX") { impl = new Impl; }

HidNX::~HidNX() { delete impl; }

void HidNX::Update() {
  HidDriver::Update();  // clear stats
}
}  // namespace PD
#else
namespace PD {

HidNX::HidNX() : HidDriver("HidNX") {}

HidNX::~HidNX() {}

void HidNX::Update() {
  HidDriver::Update();  // clear stats
}
}  // namespace PD
#endif