#pragma once

#include <pd/core/mat.hpp>
#include <pd/drivers/interface.hpp>
#include <pd/lithium/drawlist.hpp>
#include <pd/lithium/pools.hpp>
#include <pd/lithium/texture.hpp>

using PDGfxBackendFlags = PD::u32;
enum PDGfxBackendFlags_ {
  PDGfxBackendFlags_None = 0,
  PDGfxBackendFlags_FlipUV_Y = 1 << 0,     // Essential for font loading
  PDGfxBackendFlags_WindingCW = 1 << 1,    // Use CW instead of CCW winding
  PDGfxBackendFlags_HasClipRect = 1 << 2,  // Clip rects supported
  PDGfxBackendFlags_ReqPow2 = 1 << 3,      // Tex size must be pow of 2
  PDGfxBackendFlags_ApproxSDF = 1 << 4,  // Fake SDF for non smoothstep systems
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
  virtual void Draw(const Li::Drawlist& commands) {}
  Li::Texture::Ptr GetWhiteTexture() { return &pWhite; }
  PDGfxBackendFlags GetFlags() { return Flags; }

  size_t GetNumVertices() const { return CountVertices; }
  size_t GetNumIndices() const { return CountIndices; }
  size_t GetNumDrawcalls() const { return CountDrawcalls; }
  size_t GetNumCommands() const { return CountCommands; }

  // Global Pool Interface
  virtual size_t AllocateVertices(size_t count, PD::ptr accessor) = 0;
  virtual size_t AllocateIndices(size_t count, PD::ptr accessor) = 0;
  virtual bool ExpandVertices(size_t count, PD::ptr accessor) = 0;
  virtual bool ExpandIndices(size_t count, PD::ptr accessor) = 0;
  virtual void PutVertex(size_t loc, const Li::Vertex& vtx,
                         PD::ptr accessor) = 0;
  virtual void PutIndex(size_t loc, u16 idx, PD::ptr accessor) = 0;
  virtual const Li::Vertex& GetVertex(size_t loc) const = 0;
  virtual const u16& GetIndex(size_t loc) const = 0;
  virtual void ResetPools() = 0;

 protected:
  virtual void SysDeinit() {}
  virtual void SysInit() {}
  virtual void SysReset() {}
  virtual void Submit(size_t count, size_t start) {}
  virtual void UploadPools() {}  // not every driver requires it
  virtual void ClipRect() {}
  void RegisterTexture(const Li::Texture& tex);
  void UnregisterTexture(const Li::Texture& tex);

  // Counters
  size_t CountDrawcalls = 0;
  size_t CountCommands = 0;
  size_t CountVertices = 0;
  size_t CountIndices = 0;
  size_t CurrentIndex = 0;
  size_t CurrentVertex = 0;
  size_t pCountDrawcalls = 0;
  size_t pCountCommands = 0;
  // State Variables oder so
  TextureID CurrentTex = 0;
  bool CurrentTexIsSDF = false;
  bool CurrentHasClip = false;
  fvec4 CurrentClip = 0;
  Mat4 Projection;
  ivec2 ViewPort;
  std::unordered_map<TextureID, Li::Texture> pTextureRegestry;
  Li::Texture pWhite;
  PDGfxBackendFlags Flags = 0;
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

  void Draw(const Li::Drawlist& dl) override {
    const auto& commands = dl.Data();
    if (commands.size() == 0) return;
    pCountCommands += commands.size();

    size_t vtotal = dl.GetNumVertices();
    size_t itotal = dl.GetNumIndices();

    size_t start_vtx = pVtxPool.size();
    pVtxPool.Allocate(vtotal);
    CurrentVertex += vtotal;

    size_t start_idx = pIdxPool.size();
    pIdxPool.Allocate(itotal);
    CurrentIndex += itotal;

    size_t current_vtx = start_vtx;
    size_t current_idx = start_idx;

    const auto& vpool = Li::GetVertexPool();
    const auto& ipool = Li::GetIndexPool();
    /** Build Pools */
    for (size_t i = 0; i < commands.size(); i++) {
      const auto& cmd = commands[i];
      if (cmd.VertexCount > 0) {
        std::memcpy(reinterpret_cast<void*>(pVtxPool.begin() + current_vtx),
                    vpool.begin() + cmd.FirstVertex,
                    cmd.VertexCount * sizeof(Li::Vertex));
      }
      for (size_t idx = 0; idx < cmd.IndexCount; idx++) {
        u16 local_idx = ipool[cmd.FirstIndex + idx];
        pIdxPool[current_idx + idx] = static_cast<u16>(current_vtx + local_idx);
      }
      current_vtx += cmd.VertexCount;
      current_idx += cmd.IndexCount;
    }

    UploadPools();

    size_t index = 0;
    size_t ioff = start_idx;

    while (index < commands.size()) {
      CurrentTex = commands[index].Tex;
      CurrentTexIsSDF = commands[index].SDF;
      CurrentHasClip = commands[index].ClipRectUsed;
      CurrentClip = commands[index].ClipRect;
      if (!CurrentTex) {
        CurrentTex = pWhite.GetID();
      }
      size_t num_indices = 0;
      ClipRect();
      while (index < commands.size() &&
             CurrentTexIsSDF == commands[index].SDF &&
             (CurrentTex == commands[index].Tex ||
              (CurrentTex == pWhite.GetID() && commands[index].Tex == 0)) &&
             CurrentClip == commands[index].ClipRect &&
             CurrentHasClip == commands[index].ClipRectUsed) {
        num_indices += commands[index].IndexCount;
        index++;
      }
      if (num_indices > 0) {
        Submit(num_indices, ioff);
        ioff += num_indices;
        pCountDrawcalls++;
      }
    }
  }

  size_t AllocateVertices(size_t count, PD::ptr accessor) override {
    pVertexAccessor = accessor;
    size_t loc = pVtxPool.size();
    pVtxPool.Allocate(count);
    return loc;
  }

  size_t AllocateIndices(size_t count, PD::ptr accessor) override {
    pIndexAccessor = accessor;
    size_t loc = pIdxPool.size();
    pIdxPool.Allocate(count);
    return loc;
  }

  bool ExpandVertices(size_t count, PD::ptr accessor) override {
    if (pVertexAccessor != accessor) return false;
    pVtxPool.Allocate(count);
    return true;
  }

  bool ExpandIndices(size_t count, PD::ptr accessor) override {
    if (pIndexAccessor != accessor) return false;
    pIdxPool.Allocate(count);
    return true;
  }

  void PutVertex(size_t loc, const Li::Vertex& vtx, PD::ptr accessor) override {
    if (pVertexAccessor != accessor) return;
    pVtxPool.Put(loc, vtx);
  }

  void PutIndex(size_t loc, u16 idx, PD::ptr accessor) override {
    if (pIndexAccessor != accessor) return;
    pIdxPool.Put(loc, idx);
  }

  const Li::Vertex& GetVertex(size_t loc) const override {
    return pVtxPool[loc];
  }

  const u16& GetIndex(size_t loc) const override { return pIdxPool[loc]; }

  void ResetPools() override {
    pVtxPool.NoReset();
    pIdxPool.NoReset();
  }

 protected:
  u16* GetIndexBufPtr(size_t start) { return &pIdxPool[start]; }
  Li::Vertex* GetVertexBufPtr(size_t start) { return &pVtxPool[start]; }
  size_t GetVertexPoolSize() const { return pVtxPool.size(); }
  size_t GetIndexPoolSize() const { return pIdxPool.size(); }

 private:
  VtxPool pVtxPool;
  IdxPool pIdxPool;
  PD::ptr pVertexAccessor = 0;
  PD::ptr pIndexAccessor = 0;
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
  static void Draw(const Li::Drawlist& dl) { driver->Draw(dl); }
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

  static PDGfxBackendFlags GetFlags() { return driver->GetFlags(); }

  static const char* GetDriverName() { return driver->GetName(); }

  static size_t GetNumVertices() { return driver->GetNumVertices(); }
  static size_t GetNumIndices() { return driver->GetNumIndices(); }
  static size_t GetNumDrawcalls() { return driver->GetNumDrawcalls(); }
  static size_t GetNumCommands() { return driver->GetNumCommands(); }

  // Gloabal Pool Interface
  static size_t AllocateVertices(size_t count, PD::ptr accessor) {
    return driver->AllocateVertices(count, accessor);
  }

  static size_t AllocateIndices(size_t count, PD::ptr accessor) {
    return driver->AllocateIndices(count, accessor);
  }

  static bool ExpandVertices(size_t count, PD::ptr accessor) {
    return driver->ExpandVertices(count, accessor);
  }

  static bool ExpandIndices(size_t count, PD::ptr accessor) {
    return driver->ExpandIndices(count, accessor);
  }

  static void PutVertex(size_t loc, const Li::Vertex& vtx, PD::ptr accessor) {
    driver->PutVertex(loc, vtx, accessor);
  }

  static void PutIndex(size_t loc, u16 idx, PD::ptr accessor) {
    driver->PutIndex(loc, idx, accessor);
  }

  static const Li::Vertex& GetVertex(size_t loc) {
    return driver->GetVertex(loc);
  }

  static const u16& GetIndex(size_t loc) { return driver->GetIndex(loc); }

  static void NewFrame() { driver->ResetPools(); }

 private:
  static std::unique_ptr<GfxDriver> driver;
};
}  // namespace PD