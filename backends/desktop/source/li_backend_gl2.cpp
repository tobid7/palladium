#include <li_backend_gl2.hpp>

const char* vertex_shader = R"(
  #version 120
  
  attribute vec2 pos;
  attribute vec2 uv;
  attribute vec4 color;
  
  varying vec2 oUV;
  varying vec4 oColor;
  
  // Probably forgot about this matric and
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
      
  void main() {
      vec4 tc = texture2D(tex, oUV);
      gl_FragColor = tc*oColor;
  }
  )";

GLuint compileShader(const std::string& source,
                                         GLenum type) {
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

GLuint
createShaderProgram(const std::string& vertexShaderSource,
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

namespace PD {
namespace LI {
PD_BKND_DESKTOP_API void Backend_GL2::Init() {
  VertexBuffer.Resize(4 * 8192);
  IndexBuffer.Resize(6 * 8192);
  Shader = createShaderProgram(vertex_shader, frag_shader);
  glUseProgram(Shader);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  GLint _pos = glGetAttribLocation(Shader, "pos");
  GLint _uv = glGetAttribLocation(Shader, "uv");
  GLint _color = glGetAttribLocation(Shader, "color");
  glVertexAttribPointer(_pos, 2, GL_FLOAT, GL_FALSE, sizeof(PD::LI::Vertex),
                        (void*)offsetof(PD::LI::Vertex, Pos));
  glEnableVertexAttribArray(_pos);

  glVertexAttribPointer(_uv, 2, GL_FLOAT, GL_FALSE, sizeof(PD::LI::Vertex),
                        (void*)offsetof(PD::LI::Vertex, UV));
  glEnableVertexAttribArray(_uv);

  glVertexAttribPointer(_color, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                        sizeof(PD::LI::Vertex),
                        (void*)offsetof(PD::LI::Vertex, Color));
  glEnableVertexAttribArray(_color);

  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

  pLocTex = glGetUniformLocation(Shader, "tex");
  pLocProjection = glGetUniformLocation(Shader, "projection");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

PD_BKND_DESKTOP_API void Backend_GL2::Deinit() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &IBO);
}

PD_BKND_DESKTOP_API void Backend_GL2::NewFrame() {
  glViewport(0, 0, ViewPort.x, ViewPort.y);
  glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
  glClear(GL_COLOR_BUFFER_BIT);
  Projection.Ortho(0.f, ViewPort.x, ViewPort.y, 0.f, -1.f, 1.f);
  glUniformMatrix4fv(pLocProjection, 1, GL_TRUE, Projection.m);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  CurrentIndex = 0;
  CurrentVertex = 0;
  FrameCounter++;
  VertexCounter = num_vtx;
  IndexCounter = num_idx;
  num_vtx = 0;
  num_idx = 0;
}

PD_BKND_DESKTOP_API void Backend_GL2::BindTexture(PD::LI::TexAddress addr) {
  // Actually not using the Address as Address
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, (GLuint)addr);
  glUniform1i(pLocTex, 0);
}

PD_BKND_DESKTOP_API void Backend_GL2::RenderDrawData(
    const PD::Vec<PD::LI::Command::Ref>& Commands) {
  glUseProgram(Shader);
  size_t index = 0;
  while (index < Commands.Size()) {
    PD::LI::Texture::Ref Tex = Commands[index]->Tex;
    if (!Tex) {
      index++;
      continue;
    }
    size_t StartIndex = CurrentIndex;
    bool ScissorEnabled = Commands[index]->ScissorEnabled;
    ivec4 ScissorRect = Commands[index]->ScissorRect;

    while (index < Commands.Size() && Commands[index]->Tex == Tex &&
           Commands[index]->ScissorEnabled == ScissorEnabled &&
           Commands[index]->ScissorRect == ScissorRect) {
      auto c = Commands[index];
      for (size_t i = 0; i < c->IndexBuffer.Size(); i++) {
        num_idx++;
        IndexBuffer[CurrentIndex++] = CurrentVertex + c->IndexBuffer.At(i);
      }
      for (size_t i = 0; i < c->VertexBuffer.Size(); i++) {
        num_vtx++;
        VertexBuffer[CurrentVertex++] = c->VertexBuffer.At(i);
      }
      index++;
    }
    if (ScissorEnabled) {
      glScissor(ScissorRect.x, ViewPort.y - (ScissorRect.y + ScissorRect.w),
                ScissorRect.z, ScissorRect.w);
      glEnable(GL_SCISSOR_TEST);
    } else {
      glDisable(GL_SCISSOR_TEST);
    }
    BindTexture(Tex->Address);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, CurrentVertex * sizeof(PD::LI::Vertex),
                 &VertexBuffer[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, CurrentIndex * sizeof(PD::u16),
                 &IndexBuffer[0], GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, CurrentIndex - StartIndex, GL_UNSIGNED_SHORT,
                   (void*)(StartIndex * sizeof(PD::u16)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    BindTexture(0);
  }
}

PD_BKND_DESKTOP_API PD::LI::Texture::Ref Backend_GL2::LoadTexture(
    const std::vector<PD::u8>& pixels, int w, int h, PD::LI::Texture::Type type,
    PD::LI::Texture::Filter filter) {
  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);

  // Set base format (Always using RGBA as base)
  GLenum fmt = GL_RGBA;
  if (type == PD::LI::Texture::Type::RGB24) {
    fmt = GL_RGB;
  } else if (type == PD::LI::Texture::Type::A8) {
    fmt = GL_ALPHA;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE,
               pixels.data());
  if (filter == PD::LI::Texture::Filter::LINEAR) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else if (filter == PD::LI::Texture::Filter::NEAREST) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  auto res = PD::LI::Texture::New(texID, PD::ivec2(w, h));
  return res;
}

}  // namespace LI
}  // namespace PD