#pragma once

#include <pdsystem>

struct GLFWwindow;

namespace PD {
class MetalLayer {
 public:
  MetalLayer() = default;
  ~MetalLayer() = default;

  bool Create(GLFWwindow* win, GfxMetalCtx& ctx);
  void Destroy(GfxMetalCtx& ctx);
  void BeginFrame(GfxMetalCtx& ctx, const ivec2& size);
  void EndFrame(GfxMetalCtx& ctx);

 private:
  struct Impl;
  Impl* impl = nullptr;
};
}  // namespace PD
