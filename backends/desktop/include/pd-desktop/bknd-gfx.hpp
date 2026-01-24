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

#include <glad/glad.h>

/** SEPARATOR */
#include <GLFW/glfw3.h>
/** SEPARATOR */

#include <pd/lithium/lithium.hpp>

namespace PD {
class GfxGL2 : public GfxDriver {
 public:
  GfxGL2() : GfxDriver("OpenGL2") {}
  ~GfxGL2() = default;

  PD_SHARED(GfxGL2);

  void Init() override;
  void Deinit() override;
  void NewFrame() override;
  void BindTex(PD::Li::TexAddress addr) override;
  void RenderDrawData(const Li::CmdPool& Commands) override;
  PD::Li::Texture::Ref LoadTex(
      const std::vector<PD::u8>& pixels, int w, int h,
      PD::Li::Texture::Type type = PD::Li::Texture::Type::RGBA32,
      PD::Li::Texture::Filter filter =
          PD::Li::Texture::Filter::LINEAR) override;

  std::vector<Li::Vertex> VertexBuffer;
  std::vector<PD::u16> IndexBuffer;
  GLuint Shader;
  GLuint pLocProjection;
  GLuint pLocTex;
  GLuint pLocAlfa;
  GLuint VBO, IBO, VAO;  // vao is only used in > 3.3
};

class GfxGL3 : public GfxDriver2 {
 public:
  GfxGL3() : GfxDriver2("OpenGL3") {}
  ~GfxGL3() = default;

  PD_SHARED(GfxGL3);

  void Init() override;
  void Deinit() override;
  // void NewFrame() override;
  void TexBind(PD::Li::TexAddress addr) override;
  void Draw(const Li::CmdPool& pool) override;
  PD::Li::TexAddress TexLoad(
      const std::vector<PD::u8>& pixels, int w, int h,
      PD::Li::Texture::Type type = PD::Li::Texture::Type::RGBA32,
      PD::Li::Texture::Filter filter =
          PD::Li::Texture::Filter::LINEAR) override;
  void TexDelete(PD::Li::TexAddress tex) override;

  std::vector<Li::Vertex> VertexBuffer;
  std::vector<PD::u16> IndexBuffer;
  GLuint Shader;
  GLuint pLocProjection;
  GLuint pLocTex;
  GLuint pLocAlfa;
  GLuint VBO, IBO, VAO;  // vao is only used in > 3.3
};
}  // namespace PD