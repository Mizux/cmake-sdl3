#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fovDegrees, float aspectRatio, float nearPlane,
               float farPlane)
    : position(0.0f, 0.0f, 2.5f),
      target(0.0f, 0.0f, 0.0f),
      up(0.0f, 1.0f, 0.0f),
      fov(fovDegrees),
      aspectRatio(aspectRatio),
      nearPlane(nearPlane),
      farPlane(farPlane) {}

Camera::~Camera() {}

void Camera::setPosition(const glm::vec3& pos) { position = pos; }

void Camera::setTarget(const glm::vec3& targetPos) { target = targetPos; }

void Camera::setUp(const glm::vec3& upVec) { up = upVec; }

void Camera::setAspectRatio(float ratio) { aspectRatio = ratio; }

void Camera::setFov(float fovDegrees) { fov = fovDegrees; }

void Camera::setNearPlane(float np) { nearPlane = np; }

void Camera::setFarPlane(float fp) { farPlane = fp; }

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
  return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::mat4 Camera::getViewProjectionMatrix() const {
  return getProjectionMatrix() * getViewMatrix();
}
