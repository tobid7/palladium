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

#include <pd-desktop/bknd-gfx.hpp>

namespace PD {
namespace Li {
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
      
  void main() {
      vec4 tc = texture2D(tex, oUV);
      gl_FragColor = tc*oColor;
  }
  )";

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

void SetupShaderAttribs(GLuint Shader) {
  GLint _pos = glGetAttribLocation(Shader, "pos");
  GLint _uv = glGetAttribLocation(Shader, "uv");
  GLint _color = glGetAttribLocation(Shader, "color");
  glVertexAttribPointer(_pos, 2, GL_FLOAT, GL_FALSE, sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, Pos));
  glEnableVertexAttribArray(_pos);

  glVertexAttribPointer(_uv, 2, GL_FLOAT, GL_FALSE, sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, UV));
  glEnableVertexAttribArray(_uv);

  glVertexAttribPointer(_color, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                        sizeof(PD::Li::Vertex),
                        (void*)offsetof(PD::Li::Vertex, Color));
  glEnableVertexAttribArray(_color);
}

/** Actual Backend */

void GfxGL2::Init() {
  VertexBuffer.Resize(4 * 8192);
  IndexBuffer.Resize(6 * 8192);
  Shader = createShaderProgram(vertex_shader, frag_shader);
  glUseProgram(Shader);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Attribs Setup
  SetupShaderAttribs(Shader);

  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

  pLocTex = glGetUniformLocation(Shader, "tex");
  pLocProjection = glGetUniformLocation(Shader, "projection");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GfxGL2::Deinit() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &IBO);
}

void GfxGL2::NewFrame() {
  /*
  glViewport(0, 0, ViewPort.x, ViewPort.y);
  glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
  glClear(GL_COLOR_BUFFER_BIT);
  */
  Projection = Mat4::Ortho(0.f, ViewPort.x, ViewPort.y, 0.f, -1.f, 1.f);
  glUseProgram(Shader);
  glUniformMatrix4fv(pLocProjection, 1, GL_FALSE, Projection.m.data());
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  CurrentIndex = 0;
  CurrentVertex = 0;
  FrameCounter++;
  VertexCounter = NumVtx;
  IndexCounter = NumIdx;
  NumVtx = 0;
  NumIdx = 0;
}

void GfxGL2::BindTex(PD::Li::TexAddress addr) {
  // Actually not using the Address as Address
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, (GLuint)addr);
  glUniform1i(pLocTex, 0);
}

void GfxGL2::RenderDrawData(const std::vector<PD::Li::Command::Ref>& Commands) {
  size_t index = 0;
  while (index < Commands.size()) {
    PD::Li::Texture::Ref Tex = Commands[index]->Tex;
    if (!Tex) {
      index++;
      continue;
    }
    size_t StartIndex = CurrentIndex;
    bool ScissorOn = Commands[index]->ScissorOn;
    ivec4 ScissorRect = Commands[index]->ScissorRect;

    while (index < Commands.size() && Commands[index]->Tex == Tex &&
           Commands[index]->ScissorOn == ScissorOn &&
           Commands[index]->ScissorRect == ScissorRect) {
      auto c = Commands[index].get();
      for (size_t i = 0; i < c->IndexBuffer.Size(); i++) {
        NumIdx++;
        IndexBuffer[CurrentIndex++] = CurrentVertex + c->IndexBuffer.At(i);
      }
      for (size_t i = 0; i < c->VertexBuffer.Size(); i++) {
        NumVtx++;
        VertexBuffer[CurrentVertex++] = c->VertexBuffer.At(i);
      }
      index++;
    }
    if (ScissorOn) {
      glScissor(ScissorRect.x, ViewPort.y - (ScissorRect.y + ScissorRect.w),
                ScissorRect.z, ScissorRect.w);
      glEnable(GL_SCISSOR_TEST);
    } else {
      glDisable(GL_SCISSOR_TEST);
    }
    BindTex(Tex->Address);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, CurrentVertex * sizeof(PD::Li::Vertex),
                 &VertexBuffer[0], GL_DYNAMIC_DRAW);
    // For some reason we need to set these every frame for every buffer
    // Found that out when creating My 3d Engine
    SetupShaderAttribs(Shader);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, CurrentIndex * sizeof(PD::u16),
                 &IndexBuffer[0], GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLES, CurrentIndex - StartIndex, GL_UNSIGNED_SHORT,
                   (void*)(StartIndex * sizeof(PD::u16)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    BindTex(0);
  }
}

PD::Li::Texture::Ref GfxGL2::LoadTex(const std::vector<PD::u8>& pixels, int w,
                                     int h, PD::Li::Texture::Type type,
                                     PD::Li::Texture::Filter filter) {
  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);

  // Set base format (Always using RGBA as base)
  GLenum fmt = GL_RGBA;
  if (type == PD::Li::Texture::Type::RGB24) {
    fmt = GL_RGB;
  } else if (type == PD::Li::Texture::Type::A8) {
    fmt = GL_ALPHA;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE,
               pixels.data());
  if (filter == PD::Li::Texture::Filter::LINEAR) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else if (filter == PD::Li::Texture::Filter::NEAREST) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  auto res = PD::Li::Texture::New(texID, PD::ivec2(w, h));
  return res;
}
}  // namespace Li
}  // namespace PD