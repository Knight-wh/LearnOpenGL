#pragma once
#include <glm/glm.hpp>

class Light
{
public:
  // light attributes
  glm::vec3 Position;
  glm::vec3 Intensity;
  
  Light(const glm::vec3 &p, const glm::vec3 &i)
    :Position(p), Intensity(i) {};
  virtual ~Light() = default;
};