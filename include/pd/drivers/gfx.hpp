#pragma once

#include <pd/core/mat.hpp>
#include <pd/drivers/interface.hpp>
#include <pd/lithium/command.hpp>
#include <pd/lithium/texture.hpp>

using PDBackendFlags = PD::u32;
enum PDBackendFlags_ {
  PDBackendFlags_None = 0,
  PDBackendFlags_FlipUV_Y = 1 << 0,   // Essential for font loading
  PDBackendFlags_WindingCW = 1 << 0,  // Use CW instead of CCW winding
};

namespace PD {

// Pre interface class
class PD_API GfxDriver : public DriverInterface {
 public:
  GfxDriver(std::string_view name);
  virtual ~GfxDriver();

  virtual void Init() {}
  virtual void Deinit() {
    DeleteTexture(pWhite);
    SysDeinit();
  }

  void SetViewPort(const ivec2& size);
  void SetViewPort(int x, int y);
  virtual void BindTexture(TextureID id) {}
  void Reset();
  virtual Li::Texture LoadTexture(
      const std::vector<PD::u8>& pixels, int w, int h,
      TextureFormat type = TextureFormat::RGBA32,
      TextureFilter filter = TextureFilter::Linear) {
    return Li::Texture();
  }
  virtual void DeleteTexture(const Li::Texture& tex) {}
  virtual void Draw(const Pool<Li::Command>& commands) {}
  Li::Texture::Ptr GetWhiteTexture() { return &pWhite; }

 protected:
  virtual void SysDeinit() {}
  virtual void SysInit() {}
  virtual void SysReset() {}
  virtual void Submit(size_t count, size_t start) {}
  virtual void ResetPools() = 0;
  void RegisterTexture(const Li::Texture& tex);
  void UnregisterTexture(const Li::Texture& tex);

  // Counters
  size_t CountDrawcalls = 0;
  size_t CountCommands = 0;
  size_t CountVertices = 0;
  size_t CountIndices = 0;
  size_t CurrentIndex = 0;
  size_t CurrentVertex = 0;
  TextureID CurrentTex = 0;
  Mat4 Projection;
  ivec2 ViewPort;
  std::unordered_map<TextureID, Li::Texture> pTextureRegestry;
  Li::Texture pWhite;
};

struct DefaultGfxConfig {
  // Vertex Allocator
  template <typename T>
  using VertexAlloc = std::allocator<T>;
  // Index Allocator
  template <typename T>
  using IndexAlloc = std::allocator<T>;
};

template <typename Config = DefaultGfxConfig>
class GfxDriverBase : public GfxDriver {
 public:
  using VtxPool =
      Pool<Li::Vertex, typename Config::template VertexAlloc<Li::Vertex>>;
  using IdxPool = Pool<u16, typename Config::template VertexAlloc<u16>>;
  GfxDriverBase(std::string_view name = "Default") : GfxDriver(name) {}
  virtual ~GfxDriverBase() {}

  void Init() override {
    SysInit();
    std::vector<u8> img(16 * 16 * 4, 0xff);
    pWhite = LoadTexture(img, 16, 16);
  }

  void Draw(const Pool<Li::Command>& commands) override {
    CountCommands += commands.size();
    size_t index = 0;
    while (index < commands.size()) {
      CurrentTex = commands[index].Tex;
      if (!CurrentTex) {
        CurrentTex = pWhite.GetID();
      }
      size_t startidx = CurrentIndex;
      while (index < commands.size() &&
             (CurrentTex == commands[index].Tex ||
              (CurrentTex == pWhite.GetID() && commands[index].Tex == 0))) {
        const auto& c = commands[index];
        CountVertices += c.VertexCount;
        CountIndices += c.IndexCount;
        auto pIdx = pIdxPool.Allocate(c.IndexCount);
        auto pVtx = pVtxPool.Allocate(c.VertexCount);
        for (size_t i = 0; i < c.IndexCount; i++) {
          pIdx[i] = CurrentVertex + c.FirstIndex[i];
        }
        CurrentIndex += c.IndexCount;
        CurrentVertex += c.VertexCount;
        for (size_t i = 0; i < c.VertexCount; i++) {
          pVtx[i] = c.FirstVertex[i];
        }
        index++;
      }
      Submit(CurrentIndex - startidx, startidx);
    }
  }

 protected:
  u16* GetIndexBufPtr(size_t start) { return &pIdxPool[start]; }
  Li::Vertex* GetVertexBufPtr(size_t start) { return &pVtxPool[start]; }
  size_t GetVertexPoolSize() const { return pVtxPool.size(); }
  size_t GetIndexPoolSize() const { return pIdxPool.size(); }
  void ResetPools() override {
    pVtxPool.Reset();
    pIdxPool.Reset();
  }

 private:
  VtxPool pVtxPool;
  IdxPool pIdxPool;
};

class PD_API Gfx {
 public:
  Gfx() = default;
  ~Gfx() = default;

  template <typename T, typename... Args>
  static void UseDriver(Args&&... args) {
    // assert(driver == nullptr && "OS Driver already set");
    driver = std::make_unique<T>(std::forward<Args>(args)...);
  }

  static void Init() { driver->Init(); }
  static void Deinit() { driver->Deinit(); }
  static void SetViewPort(const ivec2& vp) { driver->SetViewPort(vp); }
  static void SetViewPort(int w, int h) { driver->SetViewPort(w, h); }
  static void Reset() { driver->Reset(); }
  static void Draw(const Pool<Li::Command>& commands) {
    driver->Draw(commands);
  }
  static Li::Texture LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                 int h,
                                 TextureFormat type = TextureFormat::RGBA32,
                                 TextureFilter filter = TextureFilter::Linear) {
    return driver->LoadTexture(pixels, w, h, type, filter);
  }
  static void DeleteTexture(const Li::Texture& tex) {
    driver->DeleteTexture(tex);
  }
  static Li::Texture::Ptr GetWhiteTexture() {
    return driver->GetWhiteTexture();
  }

  static const char* GetDriverName() { return driver->GetName(); }

 private:
  static std::unique_ptr<GfxDriver> driver;
};
}  // namespace PD