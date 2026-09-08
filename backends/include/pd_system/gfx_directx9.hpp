#pragma once

#include <pd/drivers/gfx.hpp>

namespace PD {
struct GfxDirectX9Config {
  // Vertex Allocator
  template <typename T>
  using VertexAlloc = std::allocator<T>;
  // Index Allocator
  template <typename T>
  using IndexAlloc = std::allocator<T>;
};

class GfxDirectX9 : public GfxDriverBase<GfxDirectX9Config> {
 public:
  GfxDirectX9(void* device = nullptr)
      : GfxDriverBase("DirectX9"), pDevice(device) {}
  ~GfxDirectX9() {}

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
  void UploadPools() override;
  void ClipRect() override;

 private:
  struct Impl;
  Impl* impl = nullptr;
  void* pDevice = nullptr;
};
}  // namespace PD