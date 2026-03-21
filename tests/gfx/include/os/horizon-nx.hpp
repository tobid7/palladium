#pragma once

#include <os-ctx.hpp>

namespace PD {
class HorizonNX : public PD::OsCtx {
 public:
  HorizonNX(Driver d = Driver::OpenGL3) : PD::OsCtx(d) {}
  ~HorizonNX() {}

  void Init() override;
  void Deinit() override;
  bool Mainloop() override;
  void ClearViewPort() override;
  void SwapBuffers() override;

 private:
  struct Impl;
  Impl* impl = nullptr;
};
}  // namespace PD