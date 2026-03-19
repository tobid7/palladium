#version 460
      
layout(location = 0) in vec2 oUV;
layout(location = 1) in vec4 oColor;

uniform sampler2D tex;
layout(push_constant) uniform PushData {
  int alfa;
} push;

out vec4 FragColor;
    
void main() {
  vec4 tc = texture(tex, oUV);
  if (push.alfa != 0) {
    FragColor = vec4(oColor.rgb, tc.a * oColor.a);
  } else {
    FragColor = tc * oColor;
  }
}