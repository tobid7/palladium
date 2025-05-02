#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#pragma once

#include <3ds.h>
#include <citro3d.h>

#include <pd/core/core.hpp>
#include <pd/lithium/backend.hpp>

namespace PD {
template <typename T>
class LinearAlloc : public Allocator<T> {
 public:
  LinearAlloc() = default;
  ~LinearAlloc() = default;

  /** Never forget the sizeof(T) again (most painful bug i created) */
  T* Allocate(size_t n) override { return (T*)linearAlloc(n * sizeof(T)); }
  void Deallocate(T* ptr) { linearFree(ptr); }
};
namespace LI {
class Backend_C3D : public PD::LI::Backend {
 public:
  Backend_C3D() : LI::Backend("Citro3D") {}
  ~Backend_C3D() {}
  PD_SMART_CTOR(Backend_C3D)

  void Init() override;

  void Deinit() override;

  void NewFrame() override;

  void BindTexture(PD::LI::TexAddress addr) override;

  void RenderDrawData(const PD::Vec<PD::LI::Command::Ref>& Commands) override;

  PD::LI::Texture::Ref LoadTexture(
      const std::vector<PD::u8>& pixels, int w, int h,
      PD::LI::Texture::Type type = PD::LI::Texture::Type::RGBA32,
      PD::LI::Texture::Filter filter =
          PD::LI::Texture::Filter::LINEAR) override;

 private:
  Vec<Vertex, LinearAlloc<Vertex>> VertexBuffer;
  Vec<u16, LinearAlloc<u16>> IndexBuffer;
  size_t CurrentVertex = 0;
  size_t CurrentIndex = 0;
  Mat4 Projection;
  int pLocProjection = 0;
  DVLB_s* ShaderCode;
  shaderProgram_s Shader;
  C3D_AttrInfo ShaderInfo;
  // For Stats
  PD::u32 num_vtx = 0;
  PD::u32 num_idx = 0;
};
}  // namespace LI
}  // namespace PD