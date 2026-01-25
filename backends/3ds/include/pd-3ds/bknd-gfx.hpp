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

#include <3ds.h>
#include <citro3d.h>

#include <pd-3ds/linearAllocator.hpp>
#include <pd/lithium/lithium.hpp>

namespace PD {
class GfxC3D : public GfxDriver {
 public:
  GfxC3D() : GfxDriver("Citro3D") {}
  ~GfxC3D() = default;

  PD_SHARED(GfxC3D);

  void Init() override;
  void Deinit() override;
  void NewFrame() override;
  void BindTex(PD::Li::TexAddress addr) override;
  void RenderDrawData(const PD::Li::CmdPool& Commands) override;
  PD::Li::Texture::Ref LoadTex(
      const std::vector<PD::u8>& pixels, int w, int h,
      PD::Li::Texture::Type type = PD::Li::Texture::Type::RGBA32,
      PD::Li::Texture::Filter filter =
          PD::Li::Texture::Filter::LINEAR) override;
  void DestroyTex(PD::Li::Texture::Ref tex) override;

  std::vector<Li::Vertex, LinearAllocator<Li::Vertex>> VertexBuffer;
  std::vector<u16, LinearAllocator<u16>> IndexBuffer;
  int pLocProjection = 0;
  DVLB_s* ShaderCode;
  shaderProgram_s Shader;
  C3D_AttrInfo ShaderInfo;
  std::vector<u8> pRawShader;
};
}  // namespace PD