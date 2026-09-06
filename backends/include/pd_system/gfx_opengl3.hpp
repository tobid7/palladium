#pragma once

#include <pd/drivers/gfx.hpp>

namespace PD {
struct GfxOpenGL3Config {
  // Vertex Allocator
  template <typename T>
  using VertexAlloc = std::allocator<T>;
  // Index Allocator
  template <typename T>
  using IndexAlloc = std::allocator<T>;

  static constexpr size_t NumVertices = 32768;  // 8192*4
  static constexpr size_t NumIndices = 49152;   // 8192*6
};

class GfxOpenGL3 : public GfxDriverBase<GfxOpenGL3Config> {
 public:
  GfxOpenGL3() : GfxDriverBase("OpenGL3") {}
  ~GfxOpenGL3() {}

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
  u32 pShader = 0;
  u32 VBO = 0;
  u32 IBO = 0;
  u32 VAO = 0;
  int pLocTex = 0;
  int pLocAlfa = 0;
  int pLocProjection = 0;
  static const char* pVertCode;
  static const char* pFragCode;
};
}  // namespace PD