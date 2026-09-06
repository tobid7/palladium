#include <pd/lithium/formatters.hpp>
#include <pd_system/gfx_opengl2.hpp>

#if defined(PD_ENABLE_OPENGL2)
#include <glad/glad.h>

#include <pd/drivers/drivers.hpp>
#include <pd_system/gl-helper.hpp>

namespace PD {

const char* GfxOpenGL2::pVertCode = R"(
  #version 120
  
  attribute vec2 pos;
  attribute vec2 uv;
  attribute vec4 color;
  
  varying vec2 oUV;
  varying vec4 oColor;
  
  // Probably forgot about this matrix and
  // searched hours for why the rendering isn't working :/
  uniform mat4 projection;
  
  void main() {
      gl_Position = projection*vec4(pos, 0.0, 1.0);
      oUV = uv;
      oColor = color;
  }
  )";

const char* GfxOpenGL2::pFragCode = R"(
  #version 120
      
  varying vec2 oUV;
  varying vec4 oColor;
  
  uniform sampler2D tex;
  uniform bool alfa;
      
  void main() {
    vec4 tc = texture2D(tex, oUV);
    if (alfa) {
      gl_FragColor = vec4(oColor.rgb, tc.a * oColor.a);
    } else {
      gl_FragColor = tc * oColor;
    }
  }
  )";

void GfxOpenGL2::pSetupShaderAttribs(u32 shader) {
  GLint _pos = glGetAttribLocation(shader, "pos");
  GLint _uv = glGetAttribLocation(shader, "uv");
  GLint _color = glGetAttribLocation(shader, "color");
  glVertexAttribPointer(_pos, 2, GL_FLOAT, GL_FALSE, sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, pos));
  glEnableVertexAttribArray(_pos);

  glVertexAttribPointer(_uv, 2, GL_FLOAT, GL_FALSE, sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, uv));
  glEnableVertexAttribArray(_uv);

  glVertexAttribPointer(_color, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                        sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, color));
  glEnableVertexAttribArray(_color);
}

void GfxOpenGL2::SysInit() {
  pShader = CreateShaderProgram(pVertCode, pFragCode);
  glUseProgram(pShader);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

  pSetupShaderAttribs(pShader);
  pLocTex = glGetUniformLocation(pShader, "tex");
  pLocAlfa = glGetUniformLocation(pShader, "alfa");
  pLocProjection = glGetUniformLocation(pShader, "projection");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GfxOpenGL2::SysDeinit() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &IBO);
  PDLOG("GfxOpenGL2::SysDeinit()");
}

void GfxOpenGL2::Submit(size_t count, size_t start) {
  BindTexture(CurrentTex);
  glUseProgram(pShader);
  glUniformMatrix4fv(pLocProjection, 1, GL_FALSE, Projection.m.data());
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, GetVertexPoolSize() * sizeof(PD::Li::Vertex),
               GetVertexBufPtr(0), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndexPoolSize() * sizeof(u16),
               GetIndexBufPtr(0), GL_DYNAMIC_DRAW);

  pSetupShaderAttribs(pShader);
  GLint ibo = 0;
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ibo);
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT,
                 reinterpret_cast<void*>(start * sizeof(u16)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  BindTexture(0);
}

void GfxOpenGL2::BindTexture(TextureID id) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, (GLuint)id);
  glUniform1i(pLocTex, 0);
  GLint fmt = 0;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &fmt);
  glUniform1i(pLocAlfa, fmt == GL_ALPHA);
}

void GfxOpenGL2::SysReset() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Li::Texture GfxOpenGL2::LoadTexture(const std::vector<PD::u8>& pixels, int w,
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
  if (type == TextureFormat::RGB24) {
    fmt = GL_RGB;
  } else if (type == TextureFormat::A8) {
    fmt = GL_ALPHA;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE,
               pixels.data());
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
  PDLOG("GfxOpenGL2::LoadTexture -> {{ {} }}, [{}, {}]", res, type, filter);
  return res;
}

void GfxOpenGL2::DeleteTexture(const Li::Texture& tex) {
  UnregisterTexture(tex);
  GLuint tex_ = tex.GetID();
  glDeleteTextures(1, &tex_);
}
}  // namespace PD
#else
namespace PD {
void GfxOpenGL2::SysInit() {
  PDERR(
      "GfxOpenGL2::SysInit: OpenGL2 Driver is not included in "
      "palladium-system");
}
void GfxOpenGL2::SysDeinit() {}
void GfxOpenGL2::Submit(size_t count, size_t start) {}
void GfxOpenGL2::BindTexture(TextureID id) {}
void GfxOpenGL2::SysReset() {}
Li::Texture GfxOpenGL2::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                    int h, TextureFormat type,
                                    TextureFilter filter) {
  return Li::Texture();
}
void GfxOpenGL2::DeleteTexture(const Li::Texture& tex) {}
void GfxOpenGL2::pSetupShaderAttribs(u32 shader) {}
}  // namespace PD
#endif