#pragma once

#include <pd/drivers/gfx.hpp>

namespace PD {
struct GfxMetalConfig {
  template <typename T>
  using VertexAlloc = std::allocator<T>;
  template <typename T>
  using IndexAlloc = std::allocator<T>;
};

struct GfxMetalCtx {
  void* Device = nullptr;   // id<MTLDevice>
  void* CmdBuf = nullptr;   // id<MTLCommandBuffer> of the current frame
  void* Encoder = nullptr;  // id<MTLRenderCommandEncoder> of the current frame
  u32 ColorFormat = 80;     // MTLPixelFormatBGRA8Unorm
  u32 DepthFormat = 0;      // MTLPixelFormatInvalid
  u32 StencilFormat = 0;    // MTLPixelFormatInvalid
  u32 SampleCount = 1;
};

class GfxMetal : public GfxDriverBase<GfxMetalConfig> {
 public:
  GfxMetal(GfxMetalCtx* ctx = nullptr) : GfxDriverBase("Metal"), pCtx(ctx) {}
  ~GfxMetal() {}

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
  GfxMetalCtx* pCtx = nullptr;
};
}  // namespace PD
