#pragma once

#include <palladium>

enum class Driver {
  Unknown = 0,
  OpenGL2 = 1,
  OpenGL3 = 2,
  DirectX9 = 3,
  Citro3D = 4,
};

namespace PD {
class OsCtx {
 public:
  OsCtx(Driver d) : pDriver(d) {}
  virtual ~OsCtx() {}

  virtual void Init() {}
  virtual void Deinit() {}
  virtual bool Mainloop() { return false; }
  virtual void ClearViewPort() {}
  virtual void SwapBuffers() {}
  PD::fvec2 PositionTranslate(PD::fvec2 in) {
    return fvec2(in.x * pViewPort.x, in.y * pViewPort.y);
  }

  PD::fvec2 SizeTranslate(PD::fvec2 in) {
    return fvec2(in.x * pViewPort.y, in.y * pViewPort.y);
  }

  const PD::ivec2& GetViewport() const { return pViewPort; }

 protected:
  PD::ivec2 pViewPort;
  const Driver pDriver;
};
}  // namespace PD