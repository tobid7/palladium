#pragma once

#include <pd/drivers/gfx.hpp>

namespace PD {
struct GfxOpenGL2Config {
  // Vertex Allocator
  template <typename T>
  using VertexAlloc = std::allocator<T>;
  // Index Allocator
  template <typename T>
  using IndexAlloc = std::allocator<T>;
};

class GfxOpenGL2 : public GfxDriverBase<GfxOpenGL2Config> {
 public:
  GfxOpenGL2() : GfxDriverBase("OpenGL2") {}
  ~GfxOpenGL2() {}

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

 private:
  void pSetupShaderAttribs(u32 shader);
  u32 pShader = 0;
  u32 VBO = 0;
  u32 IBO = 0;
  int pLocTex = 0;
  int pLocAlfa = 0;
  int pLocIsSDF = 0;
  int pLocProjection = 0;
  static const char* pVertCode;
  static const char* pFragCode;
};
}  // namespace PD