#include <pd_system/gfx_opengl3.hpp>
#if defined(PD_ENABLE_OPENGL3)
#include <glad/glad.h>

#include <pd/drivers/drivers.hpp>
#include <pd_system/gl-helper.hpp>

namespace PD {
const char* GfxOpenGL3::pVertCode = R"(
  #version 330 core
  
  layout(location = 0) in vec2 pos;
  layout(location = 1) in vec2 uv;
  layout(location = 2) in vec4 color;
  
  out vec2 oUV;
  out vec4 oColor;
  
  // Probably forgot about this matrix and
  // searched hours for why the rendering isn't working :/
  uniform mat4 projection;
  
  void main() {
      gl_Position = projection*vec4(pos, 0.0, 1.0);
      oUV = uv;
      oColor = color;
  }
  )";

const char* GfxOpenGL3::pFragCode = R"(
  #version 330 core
      
  in vec2 oUV;
  in vec4 oColor;
  
  uniform sampler2D tex;
  uniform bool alfa;

  out vec4 FragColor;
      
  void main() {
    vec4 tc = texture(tex, oUV);
    if (alfa) {
      FragColor = vec4(oColor.rgb, tc.a * oColor.a);
    } else {
      FragColor = tc * oColor;
    }
  }
  )";

void GfxOpenGL3::SysInit() {
  pShader = CreateShaderProgram(pVertCode, pFragCode);
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
  pLocAlfa = glGetUniformLocation(pShader, "alfa");
  pLocProjection = glGetUniformLocation(pShader, "projection");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  PDLOG(
      "GfxOpenGL3::SysInit():\n  pShader = {}\n  pLocTex = {}\n  pLocAlfa = "
      "{}\n  pLocProjection = {}\n  VBO = {}\n  IBO = {}, VAO = {}",
      pShader, pLocTex, pLocAlfa, pLocProjection, VBO, IBO, VAO);
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
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, CurrentVertex * sizeof(PD::Li::Vertex),
               GetVertexBufPtr(0), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, CurrentIndex * sizeof(u16),
               GetIndexBufPtr(0), GL_DYNAMIC_DRAW);

  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT,
                 reinterpret_cast<void*>(start * sizeof(u16)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  BindTexture(0);
}

void GfxOpenGL3::BindTexture(TextureID id) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, (GLuint)id);
  glUniform1i(pLocTex, 0);
  GLint fmt = 0;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &fmt);
  glUniform1i(pLocAlfa, fmt == GL_ALPHA);
}

void GfxOpenGL3::SysReset() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

TextureID GfxOpenGL3::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                  int h, TextureFormat type,
                                  TextureFilter filter) {
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
  PDLOG("GfxOpenGL3::LoadTexture -> [{}] {}, [{}, {}]", PD::ivec2(w, h), texID,
        type, filter);
  return texID;
}

void GfxOpenGL3::DeleteTexture(const TextureID& tex) {
  GLuint tex_ = tex;
  glDeleteTextures(1, &tex_);
}
}  // namespace PD
#else
namespace PD {
void GfxOpenGL3::SysInit() {
  PDLOG(
      "GfxOpenGL3::SysInit: OpenGL3 Driver is not included in "
      "palladium-system");
}
void GfxOpenGL3::SysDeinit() {}
void GfxOpenGL3::Submit(size_t count, size_t start) {}
void GfxOpenGL3::BindTexture(TextureID id) {}
void GfxOpenGL3::SysReset() {}
TextureID GfxOpenGL3::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                  int h, TextureFormat type,
                                  TextureFilter filter) {
  return 0;
}
void GfxOpenGL3::DeleteTexture(const TextureID& tex) {}
void GfxOpenGL3::pSetupShaderAttribs(u32 shader) {}
}  // namespace PD
#endif