#include <glad/glad.h>

#include <iostream>
#include <pd_system/gl-helper.hpp>

namespace PD {
GLuint compileShader(const char* src, GLenum type) {
  GLuint shader = glCreateShader(type);
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
u32 CreateShaderProgram(const char* vert, const char* frag) {
  GLuint vertexShader = compileShader(vert, GL_VERTEX_SHADER);
  GLuint fragmentShader = compileShader(frag, GL_FRAGMENT_SHADER);

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

  if (success) PDLOG("Shader [{}] compiled sucessfully", shaderProgram);

  return shaderProgram;
}
}  // namespace PD