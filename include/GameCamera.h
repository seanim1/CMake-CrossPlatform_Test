#pragma once

#include <glm/glm.hpp>

class GameCamera {
public:
    GameCamera(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);

    void SetPosition(const glm::vec3& pos);
    void SetTarget(const glm::vec3& tgt);
    void SetAspectRatio(float ratio);
    glm::vec3 GetPosition() const;
    glm::vec3 GetDirection() const;
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    float fov;
    float aspect;
    float nearClip;
    float farClip;

    glm::mat4 view;
    glm::mat4 proj;
};
