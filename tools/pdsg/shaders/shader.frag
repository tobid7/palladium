#version 460
      
layout(location = 0) in vec2 oUV;
layout(location = 1) in vec4 oColor;

uniform sampler2D tex;
layout(push_constant) uniform PushData {
  int alfa;
  int is_sdf;
} push;

out vec4 FragColor;
    
void main() {
  vec4 tc = texture(tex, oUV);
  if(push.is_sdf != 0) {
    float alpha = smoothstep(0.45, 0.55, tc.a);
    FragColor = vec4(oColor.rgb, alpha * oColor.a);
  } else if (push.alfa != 0) {
    FragColor = vec4(oColor.rgb, tc.a * oColor.a);
  } else {
    FragColor = tc * oColor;
  }
}