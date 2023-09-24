#version 460 core
out vec4 FragColor;

// TODO add time
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
  vec3 boxOrigin = vec3(1.0, 1.0, 1.0);
  sdField = boxSDF(p, boxOrigin);
  // vec3 sphereOrigin = vec3(0.0, 0.0, 0.0);
  // sdField = sphereSDF(p - sphereOrigin, 1.0);
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

/*
 * 估算表面法向量
 */
vec3 estimateNormal(vec3 p) {
  return normalize(vec3(
    sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
    sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
    sceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
  ));
}

/*
 * 计算光照使用blinn-Phong模型
 */
vec3 calLightWithBlinn(vec3 ka, vec3 kd, vec3 ks, float alpha, vec3 point, vec3 cameraPos) {
  const vec3 objectColor = vec3(1.0, 1.0, 1.0);
  const vec3 lightColor = vec3(1.0, 1.0, 0.0);
  const vec3 lightPos = vec3(4.0, 4.0, 4.0);

  vec3 norm = estimateNormal(point);

  // Phong Shder
  // ambient
	vec3 ambient = ka * lightColor;
	
	// diffuse
	vec3 lightDir = normalize(lightPos - point);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = kd * diff * lightColor;
	
	// specular
	vec3 viewDir = normalize(cameraPos - point);
  vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32);
	vec3 specular = ks * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;

  return result;
}

/*
 * 计算视图矩阵
 */
mat4 viewMatrix(vec3 camera, vec3 center, vec3 up) {
  vec3 f = normalize(center - camera);
  vec3 s = normalize(cross(f, up));
  vec3 u = cross(s, f);
  return mat4(
    vec4(s, 0.0),
    vec4(u, 0.0),
    vec4(-f, 0.0),
    vec4(0.0, 0.0, 0.0, 1.0)
  );
}


void main() {
  // 相机坐标
  vec3 cameraPos = vec3(-2.0, 4.0, 5.0);
  vec3 scenePos = vec3(1.0, 0.0, 0.0);
  vec3 cameraUp = vec3(0.0, 1.0, 0.0);
  mat4 viewMat = viewMatrix(cameraPos, scenePos, cameraUp);

  vec3 rayDir = rayDirection(fov, iResolution, gl_FragCoord.xy);
  rayDir = (viewMat * vec4(rayDir, 0.0)).xyz;

  float dist = shortestDistanceToSurface(cameraPos, rayDir, MIN_DIST, MAX_DIST);
  if (dist > MAX_DIST - EPSILON) {
    // didn't hit anything
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

  // The closest point on the surface to the eyepoint along the view ray
  vec3 HitPos = cameraPos + rayDir * dist;

  vec3 ka = vec3(0.1);
  vec3 kd = vec3(0.9);
  vec3 ks = vec3(0.5);
  
  vec3 color = calLightWithBlinn(ka, kd, ks, 1.0, HitPos, cameraPos);

  FragColor = vec4(color, 1.0);
}