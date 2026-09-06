#pragma once

#include <pd/core/mat.hpp>
#include <pd/drivers/interface.hpp>
#include <pd/lithium/command.hpp>

using LiBackendFlags = PD::u32;
enum LiBackendFlags_ {
  LiBackendFlags_None = 0,
  LiBackendFlags_FlipUV_Y = 1 << 0,  // Essential for font loading
};

namespace PD {
using TextureID = ptr;
enum class TextureFilter {
  Linear,
  Nearest,
};

enum class TextureFormat {
  RGBA32,
  A8,
};

// Pre interface class
class PD_API GfxDriver : public DriverInterface {
 public:
  GfxDriver(std::string_view name);
  virtual ~GfxDriver() = default;

  virtual void Init() {}
  virtual void Deinit() { SysDeinit(); }

  void SetViewPort(const ivec2& size);
  void SetViewPort(int x, int y);
  virtual void BindTexture(TextureID id) {}
  void Reset();
  virtual TextureID LoadTexture(const std::vector<PD::u8>& pixels, int w, int h,
                                TextureFormat type = TextureFormat::RGBA32,
                                TextureFilter filter = TextureFilter::Linear) {
    return 0;
  }
  virtual void DeleteTexture(const TextureID& tex) {}
  virtual void Draw(const Pool<Li::Command>& commands) {}

 protected:
  virtual void SysDeinit() {}
  virtual void SysInit() {}
  virtual void SysReset() {}
  virtual void Submit(size_t count, size_t start) {}
  virtual void ResetPools() = 0;

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
};

struct DefaultGfxConfig {
  // Vertex Allocator
  template <typename T>
  using VertexAlloc = std::allocator<T>;
  // Index Allocator
  template <typename T>
  using IndexAlloc = std::allocator<T>;
  using IndexType = u16;  // Index Type

  static constexpr size_t NumVertices = 32768;  // 8192*4
  static constexpr size_t NumIndices = 49152;   // 8192*6
};

template <typename Config = DefaultGfxConfig>
class PD_API GfxDriverBase : public GfxDriver {
 public:
  using IndexType = Config::IndexType;
  using VtxPool =
      Pool<Li::Vertex, typename Config::template VertexAlloc<Li::Vertex>>;
  using IdxPool =
      Pool<IndexType, typename Config::template VertexAlloc<IndexType>>;
  GfxDriverBase(std::string_view name = "Default") : GfxDriver(name) {}
  virtual ~GfxDriverBase() {}

  void Init() override {
    pVtxPool.Init(Config::NumVertices);
    pIdxPool.Init(Config::NumIndices);
    SysInit();
  }

  void Draw(const Pool<Li::Command>& commands) override {
    CountCommands += commands.size();
    size_t index = 0;
    while (index < commands.size()) {
      CurrentTex = commands[index].Tex;
      if (!CurrentTex) {
        index++;
        continue;
      }
      size_t startidx = CurrentIndex;
      while (index < commands.size() && CurrentTex == commands[index].Tex) {
        const auto& c = commands[index];
        CountVertices += c.VertexCount;
        CountIndices += c.IndexCount;
        auto pIdx = pIdxPool.Allocate(c.IndexCount);
        auto pVtx = pVtxPool.Allocate(c.VertexCount);
        for (size_t i = 0; i < c.IndexCount; i++) {
          pIdx[i] = CurrentVertex + c.FirstIndex[i];
        }
        for (size_t i = 0; i < c.VertexCount; i++) {
          pVtx[i] = c.FirstVertex[i];
        }
        index++;
      }
      Submit(index - startidx, startidx);
    }
  }

 protected:
  IndexType* GetIndexBufPtr(size_t start) { return &pIdxPool[start]; }
  Li::Vertex* GetVertexBufPtr(size_t start) { return &pVtxPool[start]; }
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
  static TextureID LoadTexture(const std::vector<PD::u8>& pixels, int w, int h,
                               TextureFormat type = TextureFormat::RGBA32,
                               TextureFilter filter = TextureFilter::Linear) {
    return driver->LoadTexture(pixels, w, h, type, filter);
  }

  static const char* GetDriverName() { return driver->GetName(); }

 private:
  static std::unique_ptr<GfxDriver> driver;
};
}  // namespace PD