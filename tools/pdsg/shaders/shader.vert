#version 460
  
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;

layout(location = 0) out vec2 oUV;
layout(location = 1) out vec4 oColor;

// Probably forgot about this matrix and
// searched hours for why the rendering isn't working :/
layout(set = 0, binding = 0) uniform UBO {
  mat4 projection;
} ubo;

void main() {
    gl_Position = ubo.projection*vec4(pos, 0.0, 1.0);
    oUV = uv;
    oColor = color;
}