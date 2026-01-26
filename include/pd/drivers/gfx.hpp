#pragma once

/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <pd/core/core.hpp>
#include <pd/drivers/types.hpp>
#include <pd/lithium/command.hpp>
#include <pd/lithium/texture.hpp>

using LiBackendFlags = PD::u32;
enum LiBackendFlags_ {
  LiBackendFlags_None = 0,
  LiBackendFlags_FlipUV_Y = PD_BIT(0),  // Essential for font loading
};

namespace PD {
// The backend api nobody asked for
class GfxDriver2 {
 public:
  GfxDriver2(const std::string& name = "NullGfx") : pName(name) {};
  ~GfxDriver2() = default;

  virtual void Init() {}
  virtual void Deinit() {}
  virtual void Draw(const PD::Li::CmdPool& pool) {}
  virtual void TexBind(PD::Li::TexAddress addr) {}
  virtual PD::Li::TexAddress TexLoad(
      const std::vector<PD::u8>& pixels, int w, int h,
      PD::Li::Texture::Type type = PD::Li::Texture::Type::RGBA32,
      PD::Li::Texture::Filter filter = PD::Li::Texture::Filter::LINEAR) {
    return 0;
  }
  virtual void TexDelete(PD::Li::TexAddress tex) {}

 protected:
  const std::string pName;
  LiBackendFlags pFlags = 0;
  size_t CurrentIndex = 0;
  size_t CurrentVertex = 0;
  ivec2 ViewPort;
};
class GfxDriver {
 public:
  GfxDriver(const std::string& name = "NullGfx") : pName(name) {}
  GfxDriver(PDDriverData data) : pName("NullGfx") {}
  ~GfxDriver() = default;

  PD_SHARED(GfxDriver);

  void PostInit();

  virtual void Init() {}
  virtual void Deinit() {}
  virtual void NewFrame() {}

  virtual void BindTex(Li::TexAddress addr) {}

  virtual void RenderDrawData(const Li::CmdPool& Commands) {}

  void SetViewPort(const ivec2& vp) { ViewPort = vp; }
  void SetViewPort(int w, int h) { ViewPort = PD::ivec2(w, h); }

  virtual Li::Texture::Ref LoadTex(
      const std::vector<u8>& pixels, int w, int h,
      Li::Texture::Type type = Li::Texture::Type::RGBA32,
      Li::Texture::Filter filter = Li::Texture::Filter::LINEAR) {
    // Texture loading not supported (when this func not get override)
    return nullptr;
  }

  virtual void DestroyTex(PD::Li::Texture::Ref tex) {}

  Li::Texture::Ref GetSolidTex() { return pSolid; }

  const std::string& GetName() const { return pName; }

  const std::string pName = "NullGfx";
  LiBackendFlags Flags = 0;
  ivec2 ViewPort;
  Mat4 Projection;
  Li::Texture::Ref pSolid;
  size_t CurrentVertex = 0;
  size_t CurrentIndex = 0;

  /** Debug Variables */

  // Optional Index counter
  u32 IndexCounter;
  // Optional Vertex counter
  u32 VertexCounter;
  // Optional Frame Counter
  u64 FrameCounter;
};
}  // namespace PD