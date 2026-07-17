#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:
  Camera(float fovDegrees = 45.0f, float aspectRatio = 1280.0f / 800.0f,
         float nearPlane = 0.1f, float farPlane = 100.0f);
  ~Camera();

  void setPosition(const glm::vec3& pos);
  void setTarget(const glm::vec3& targetPos);
  void setUp(const glm::vec3& upVec);
  void setAspectRatio(float ratio);
  void setFov(float fovDegrees);
  void setNearPlane(float nearPlane);
  void setFarPlane(float farPlane);

  glm::vec3 getPosition() const { return position; }
  glm::vec3 getTarget() const { return target; }
  glm::vec3 getUp() const { return up; }
  float getFov() const { return fov; }
  float getAspectRatio() const { return aspectRatio; }
  float getNearPlane() const { return nearPlane; }
  float getFarPlane() const { return farPlane; }

  glm::mat4 getViewMatrix() const;
  glm::mat4 getProjectionMatrix() const;
  glm::mat4 getViewProjectionMatrix() const;

private:
  glm::vec3 position;
  glm::vec3 target;
  glm::vec3 up;

  float fov;
  float aspectRatio;
  float nearPlane;
  float farPlane;
};

#endif // CAMERA_HPP
