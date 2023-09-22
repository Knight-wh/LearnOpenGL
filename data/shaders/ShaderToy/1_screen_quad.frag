#version 460 core
out vec4 FragColor;

uniform vec2 iResolution;

float drawCircle(vec2 uv, vec2 center, float radius, float blur) {
  float d = length(uv - center);
  float res = smoothstep(radius, radius-blur, d);
  return res;
}

void main() {
  // 归一化屏幕像素坐标
  vec2 uv = gl_FragCoord.xy/iResolution.xy;
  uv -= 0.5;
  uv.x *= iResolution.x/iResolution.y;

  float mask = drawCircle(uv, vec2(0.0), 0.4, 0.05);

  // draw eyes
  mask -= drawCircle(uv, vec2(-.12, .155), .07, .02);
  mask -= drawCircle(uv, vec2(.12, .155), .07, .02);

  // draw mouth
  float mouth = drawCircle(uv, vec2(0., 0.), .3, .02);
  mouth -= drawCircle(uv, vec2(0., .1), .3, .02);
  
  mask -= mouth;

  vec3 col = vec3(1.0, 1.0, 0.0) * mask;
  FragColor = vec4(col, 1.0);
}