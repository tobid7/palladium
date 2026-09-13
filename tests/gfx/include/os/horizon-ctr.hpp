#pragma once

#include <os-ctx.hpp>

namespace PD {
class HorizonCtr : public PD::OsCtx {
 public:
  HorizonCtr(Driver d = Driver::Citro3D) : PD::OsCtx(d) {}
  ~HorizonCtr() {}

  void Init() override;
  void Deinit() override;
  bool Mainloop() override;
  void ClearViewPort() override;
  void SwapBuffers() override;
  int GetScreenCount() override { return 2; }
  void DrawOnScreen(int s) override;

 private:
  struct Impl;
  Impl* impl = nullptr;
};
}  // namespace PD