#include <pd/drivers/gfx.hpp>

namespace PD {
PD_API std::unique_ptr<GfxDriver> Gfx::driver;

GfxDriver::GfxDriver(std::string_view name) : DriverInterface(name) {}

void GfxDriver::SetViewPort(const ivec2& size) { ViewPort = size; }
void GfxDriver::SetViewPort(int x, int y) {
  ViewPort.x = x;
  ViewPort.y = y;
}

void GfxDriver::Reset() {
  CurrentVertex = 0;
  CurrentIndex = 0;
  SysReset();
}
}  // namespace PD