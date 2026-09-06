#pragma once

#include <pd/drivers/gfx.hpp>
#include <pd_system/ctr-linear-allocator.hpp>

namespace PD {
struct GfxCitro3DConfig {
  // Vertex Allocator
  template <typename T>
  using VertexAlloc = LinearAllocator<T>;
  // Index Allocator
  template <typename T>
  using IndexAlloc = LinearAllocator<T>;
};

class GfxCitro3D : public GfxDriverBase<GfxCitro3DConfig> {
 public:
  GfxCitro3D() : GfxDriverBase("Citro3D") {}
  ~GfxCitro3D() {}

  void SysInit() override;
  void SysDeinit() override;
  void Submit(size_t count, size_t start) override;
  void BindTexture(TextureID id) override;
  void SysReset() override;
  Li::Texture LoadTexture(
      const std::vector<PD::u8>& pixels, int w, int h,
      TextureFormat type = TextureFormat::RGBA32,
      TextureFilter filter = TextureFilter::Linear) override;
  void DeleteTexture(const Li::Texture& tex) override;

 private:
  struct Impl;
  Impl* impl = nullptr;
};
}  // namespace PD