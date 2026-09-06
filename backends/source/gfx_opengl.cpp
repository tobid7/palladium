#include <glad/glad.h>

#include <gfx_opengl.hpp>

namespace PD {
GLuint compileShader(const std::string& source, GLenum type) {
  GLuint shader = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
  }

  return shader;
}

GLuint createShaderProgram(const std::string& vertexShaderSource,
                           const std::string& fragmentShaderSource) {
  GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShader =
      compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cerr << "Shader Program Linking Error: " << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

const char* vertex_shader = R"(
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

const char* frag_shader = R"(
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

void GfxOpenGL::pSetupShaderAttribs(u32 shader) {
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

void GfxOpenGL::SysInit() {
  pShader = createShaderProgram(vertex_shader, frag_shader);
  glUseProgram(pShader);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  pSetupShaderAttribs(pShader);
  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

  pLocTex = glGetUniformLocation(pShader, "tex");
  pLocAlfa = glGetUniformLocation(pShader, "alfa");
  pLocProjection = glGetUniformLocation(pShader, "projection");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GfxOpenGL::SysDeinit() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &IBO);
}

void GfxOpenGL::Submit(size_t count, size_t start) {
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT,
                 (void*)GetIndexBufPtr(start));
}

void GfxOpenGL::BindTexture(TextureID id) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, (GLuint)id);
  glUniform1i(pLocTex, 0);
  GLint fmt = 0;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &fmt);
  glUniform1i(pLocAlfa, fmt == GL_ALPHA);
}

void GfxOpenGL::SysReset() {}

TextureID GfxOpenGL::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                 int h, TextureFormat type,
                                 TextureFilter filter) {
  return 0;
}

void GfxOpenGL::DeleteTexture(const TextureID& tex) {
  GLuint tex_ = tex;
  glDeleteTextures(1, &tex_);
}
}  // namespace PD