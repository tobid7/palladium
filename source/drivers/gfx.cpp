#include <pd/drivers/gfx.hpp>
#include <pd/lithium/formatters.hpp>

namespace PD {
PD_API std::unique_ptr<GfxDriver> Gfx::driver;

PD_API GfxDriver::GfxDriver(std::string_view name) : DriverInterface(name) {}

PD_API GfxDriver::~GfxDriver() {
  if (pTextureRegestry.size()) {
    PDERR("GfxDriver: {} is still holding {} texture{}!", GetName(),
          pTextureRegestry.size(), (pTextureRegestry.size() == 1 ? "" : "s"));
  }
}

PD_API void GfxDriver::SetViewPort(const ivec2& size) {
  ViewPort = size;
  Projection = Mat4::Ortho(0.f, ViewPort.x, ViewPort.y, 0.f, 1.f, -1.f);
}

PD_API void GfxDriver::SetViewPort(int x, int y) {
  ViewPort.x = x;
  ViewPort.y = y;
  Projection = Mat4::Ortho(0.f, ViewPort.x, ViewPort.y, 0.f, 1.f, -1.f);
}

PD_API void GfxDriver::Reset() {
  CountIndices = CurrentIndex;
  CountVertices = CurrentVertex;
  CurrentVertex = 0;
  CurrentIndex = 0;
  CountCommands = pCountCommands;
  CountDrawcalls = pCountDrawcalls;
  pCountCommands = 0;
  pCountDrawcalls = 0;
  SysReset();
}

PD_API void GfxDriver::RegisterTexture(const Li::Texture& tex) {
  pTextureRegestry[tex.GetID()] = tex;
}

PD_API void GfxDriver::UnregisterTexture(const Li::Texture& tex) {
  if (pTextureRegestry.count(tex.GetID())) {
    pTextureRegestry.erase(pTextureRegestry.find(tex.GetID()));
    PDLOG("GfxDriver: Texture {{ {} }} has been deleted!", tex);
  } else {
    PDWARN("GfxDriver: WARNING Texture {{ {} }} does not exist in regestry!",
           tex);
  }
}
}  // namespace PD