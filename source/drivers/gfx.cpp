#include <pd/drivers/gfx.hpp>

namespace PD {
PD_API std::unique_ptr<GfxDriver> Gfx::driver;

PD_API GfxDriver::GfxDriver(std::string_view name) : DriverInterface(name) {}

PD_API void GfxDriver::SetViewPort(const ivec2& size) {
  ViewPort = size;
  Projection = Mat4::Ortho(0.f, ViewPort.x, ViewPort.y, 0.f, 1.f, -1.f);
}

PD_API  void GfxDriver::SetViewPort(int x, int y) {
  ViewPort.x = x;
  ViewPort.y = y;
  Projection = Mat4::Ortho(0.f, ViewPort.x, ViewPort.y, 0.f, 1.f, -1.f);
}

PD_API void GfxDriver::Reset() {
  CurrentVertex = 0;
  CurrentIndex = 0;
  ResetPools();
  SysReset();
}
}  // namespace PD