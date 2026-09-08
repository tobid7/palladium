#include <pd/lithium/formatters.hpp>
#include <pd_system/gfx_opengl3.hpp>

#if defined(PD_ENABLE_OPENGL3)
#include <glad/glad.h>

#include <pd/drivers/drivers.hpp>
#include <pd_system/gl-helper.hpp>
#include <pd_system/shaders.hpp>
#include <spirv-helper.hpp>

namespace PD {
void GfxOpenGL3::SysInit() {
  std::string vcode = SpirvHelper::SPV2GLSL(
      std::vector<unsigned int>(
          Shaders::VertexShader,
          Shaders::VertexShader +
              (sizeof(Shaders::VertexShader) / sizeof(unsigned int))),
      330, false);
  std::string fcode = SpirvHelper::SPV2GLSL(
      std::vector<unsigned int>(
          Shaders::FragmentShader,
          Shaders::FragmentShader +
              (sizeof(Shaders::FragmentShader) / sizeof(unsigned int))),
      330, false);
  PDLOG("Vertex: \n{}", vcode);
  PDLOG("Fragment: \n{}", fcode);
  pShader = CreateShaderProgram(vcode.c_str(), fcode.c_str());
  glUseProgram(pShader);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, uv));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, color));

  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

  pLocTex = glGetUniformLocation(pShader, "tex");
  pLocAlfa = glGetUniformLocation(pShader, "push.alfa");
  pLocIsSDF = glGetUniformLocation(pShader, "push.is_sdf");
  pLocProjection = glGetUniformLocation(pShader, "ubo.projection");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void GfxOpenGL3::SysDeinit() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &IBO);
  glDeleteVertexArrays(1, &VAO);
  PDLOG("GfxOpenGL3::SysDeinit()");
}

void GfxOpenGL3::Submit(size_t count, size_t start) {
  BindTexture(CurrentTex);
  glUseProgram(pShader);
  glUniformMatrix4fv(pLocProjection, 1, GL_FALSE, Projection.m.data());

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT,
                 reinterpret_cast<void*>(start * sizeof(u16)));
  glBindVertexArray(0);

  BindTexture(0);
}

void GfxOpenGL3::BindTexture(TextureID id) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, (GLuint)id);
  glUniform1i(pLocTex, 0);
  GLint fmt = 0;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &fmt);
  glUniform1i(pLocAlfa, fmt == GL_R8);
  glUniform1i(pLocIsSDF, CurrentTexIsSDF);
}

void GfxOpenGL3::SysReset() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Li::Texture GfxOpenGL3::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                    int h, TextureFormat type,
                                    TextureFilter filter) {
  if (pixels.size() !=
      static_cast<size_t>(w * h * Li::TextureFormat2Bpp(type))) {
    PDERR("Failed to load Texture due to Size mismatch: {} != {}",
          pixels.size(),
          static_cast<size_t>(w * h * Li::TextureFormat2Bpp(type)));
    return Li::Texture();
  }
  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);

  // Set base format (Always using RGBA as base)
  GLenum fmt = GL_RGBA;
  GLenum ifmt = GL_RGBA8;
  if (type == TextureFormat::RGB24) {
    ifmt = GL_RGB8;
    fmt = GL_RGB;
  } else if (type == TextureFormat::A8) {
    ifmt = GL_R8;
    fmt = GL_RED;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, ifmt, w, h, 0, fmt, GL_UNSIGNED_BYTE,
               pixels.data());

  if (type == TextureFormat::A8) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
  }
  if (filter == TextureFilter::Linear) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else if (filter == TextureFilter::Nearest) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  Li::Texture res;
  res.SetID(texID);
  res.SetSize(w, h);
  res.SetUV(0.f, 0.f, 1.f, 1.f);
  RegisterTexture(res);
  PDLOG("GfxOpenGL3::LoadTexture -> {{ {} }}, [{}, {}]", res, type, filter);
  return res;
}

void GfxOpenGL3::DeleteTexture(const Li::Texture& tex) {
  UnregisterTexture(tex);
  GLuint tex_ = tex.GetID();
  glDeleteTextures(1, &tex_);
}

void GfxOpenGL3::UploadPools() {
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, GetVertexPoolSize() * sizeof(PD::Li::Vertex),
               GetVertexBufPtr(0), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndexPoolSize() * sizeof(u16),
               GetIndexBufPtr(0), GL_DYNAMIC_DRAW);

  glBindVertexArray(0);
}

void GfxOpenGL3::ClipRect() {
  if (CurrentHasClip) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(CurrentClip.x, ViewPort.y - (CurrentClip.y + CurrentClip.w),
              CurrentClip.z, CurrentClip.w);
  } else {
    glDisable(GL_SCISSOR_TEST);
  }
}
}  // namespace PD
#else
namespace PD {
void GfxOpenGL3::SysInit() {
  PDERR(
      "GfxOpenGL3::SysInit: OpenGL3 Driver is not included in "
      "palladium-system");
}
void GfxOpenGL3::SysDeinit() {}
void GfxOpenGL3::Submit(size_t count, size_t start) {}
void GfxOpenGL3::BindTexture(TextureID id) {}
void GfxOpenGL3::SysReset() {}
Li::Texture GfxOpenGL3::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                    int h, TextureFormat type,
                                    TextureFilter filter) {
  return Li::Texture();
}
void GfxOpenGL3::DeleteTexture(const Li::Texture& tex) {}
void GfxOpenGL3::UploadPools() {}
void GfxOpenGL3::ClipRect() {}
}  // namespace PD
#endif