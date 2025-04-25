#pragma once

#include <glm/glm.hpp>

class Kinematic {
public:
    glm::vec3 GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos) { position = pos; }

    glm::vec3 GetVelocity() const { return velocity; }
    void SetVelocity(const glm::vec3& vel) { velocity = vel; }

    void Update(float dt) {
        position += velocity * dt;
    }

protected:
    glm::vec3 position{ 0.0f };
    glm::vec3 velocity{ 0.0f };
};
