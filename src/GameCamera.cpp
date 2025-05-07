#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "GameCamera.h"
#include <glm/gtc/matrix_transform.hpp>

GameCamera::GameCamera(float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
    : position(0.0f, 0.0f, 5.0f),
    target(0.0f, 0.0f, 0.0f),
    up(0.0f, 1.0f, 0.0f),
    fov(glm::radians(fovDegrees)),
    aspect(aspectRatio),
    nearClip(nearPlane),
    farClip(farPlane)
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void GameCamera::SetPosition(const glm::vec3& pos) {
    position = pos;
    UpdateViewMatrix();
}

void GameCamera::SetTarget(const glm::vec3& tgt) {
    target = tgt;
    UpdateViewMatrix();
}

void GameCamera::SetAspectRatio(float ratio) {
    aspect = ratio;
    UpdateProjectionMatrix();
}
glm::vec3 GameCamera::GetPosition() const {
    return position;
}

glm::vec3 GameCamera::GetDirection() const
{
    return glm::normalize(target - position);
}

const glm::mat4& GameCamera::GetViewMatrix() const {
    return view;
}

const glm::mat4& GameCamera::GetProjectionMatrix() const {
    return proj;
}

void GameCamera::UpdateViewMatrix() {
    view = glm::lookAt(position, target, up);
}

void GameCamera::UpdateProjectionMatrix() {
    proj = glm::perspective(fov, aspect, nearClip, farClip);
    proj[1][1] *= -1.0f; // Flip Y for Vulkan
}
