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

class PD_API GfxDriver : public DriverInterface {
 public:
  GfxDriver(std::string_view name = "Default") : DriverInterface(name) {}
  virtual ~GfxDriver() {}

  virtual void Init() {}
  virtual void Deinit() {}
  virtual void Submit() {}
  virtual void BindTexture(TextureID id) {}
  virtual void Reset() {}
  virtual TextureID LoadTexture(const std::vector<PD::u8>& pixels, int w, int h,
                                TextureFormat type = TextureFormat::RGBA32,
                                TextureFilter filter = TextureFilter::Linear) {}
  virtual void DeleteTexture(const TextureID& tex) {}
  void Draw(const std::vector<Li::Command>& commands) {
    CountCommands += commands.size();
    // Bind shader
    Projection = Mat4::Ortho(0.f, ViewPort.x, ViewPort.y, 0.f, 1.f, -1.f);
    // Shader Set Mtx
    size_t index = 0;
    while (index < commands.size()) {
      CurrentTex = commands[index].Tex;
      if (!CurrentTex) {
        index++;
        continue;
      }
    }
  }

 private:
  size_t CurrentIndex = 0;
  size_t CurrentVertex = 0;
  TextureID CurrentTex = 0;
  Mat4 Projection;
  ivec2 ViewPort;

  // Counters
  size_t CountDrawcalls = 0;
  size_t CountCommands = 0;
  size_t CountVertices = 0;
  size_t CountIndices = 0;
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

 private:
  static std::unique_ptr<GfxDriver> driver;
};
}  // namespace PD