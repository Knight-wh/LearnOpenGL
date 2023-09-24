#version 460 core
out vec4 FragColor;

uniform vec2 iResolution;

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;
const float fov = 45.0;

/*
 * sphere's sd，球体的质心在原点
 * p: 空间中某点，s代表球体半径
 */
float sphereSDF(vec3 p, float s) {
  return length(p) - s;
}

/*
 * box's sd， box的质心在原点
 * p: 空间中某点，b代表box的+x, +y, +z的顶点
 */
float boxSDF(vec3 p, vec3 b) {
  vec3 q = abs(p) - b;
  // ? 这里是怎么实现的
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

/*
 * 用SDF描述场景
 */
float sceneSDF(vec3 p) {
  float sdField;
  // vec3 boxOrigin = vec3(1.0, 1.0, 1.0);
  // sdField = boxSDF(p, boxOrigin);
  vec3 sphereOrigin = vec3(0.0, 0.0, 0.0);
  sdField = sphereSDF(p - sphereOrigin, 1.0);
  return sdField;
}

/*
 * 返回最短距离函数
 * eye: 射线发射点
 * marchingDir: 射线的方向，标准化
 * start: 从相机开始的最短距离
 * end: 最远距离
 * return: range[MIN_DIST, MAX_DIST]
 */
float shortestDistanceToSurface(vec3 eye, vec3 marchingDir, float start, float end) {
  float depth = start;
  for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
    float dist = sceneSDF(eye + marchingDir * depth);
    if (dist < EPSILON) {
      return depth;
    }
    depth += dist;
    if (depth >= end) {
      return end;
    }
  }
  return end;
}

/*
 * 射线的标准化方向向量
 */
vec3 rayDirection(float fov, vec2 size, vec2 fragCoord) {
  float imageAspectRatio = size.x / size.y;
  vec2 uv = 2 * fragCoord.xy / size.xy; // [0, 2]
  uv -= 1.0; // [-1, 1]
  uv *= tan(radians(fov) * 0.5);
  uv.x *= imageAspectRatio;
  return normalize(vec3(uv, -1.0));
}

void main() {
  // 相机坐标
  vec3 rayOrigin = vec3(0.0, 0.0, 5.0);
  vec3 rayDir = rayDirection(fov, iResolution, gl_FragCoord.xy);

  float dist = shortestDistanceToSurface(rayOrigin, rayDir, MIN_DIST, MAX_DIST);
  if (dist > MAX_DIST - EPSILON) {
    // didn't hit anything
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }
  FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}