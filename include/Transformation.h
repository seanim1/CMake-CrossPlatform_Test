#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transformation {
protected:
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f }; // Euler angles in radians
    glm::vec3 scale{ 1.0f };
    glm::mat4 modelMatrix{ 1.0f };
    bool dirty = true;

    void updateModelMatrix() {
        if (!dirty) return;

        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
        modelMatrix = glm::scale(modelMatrix, scale);

        dirty = false;
    }

public:
    virtual ~Transformation() = default;

    void setPosition(const glm::vec3& pos) {
        position = pos;
        dirty = true;
    }

    void setRotation(const glm::vec3& rot) {
        rotation = rot;
        dirty = true;
    }

    void setScale(const glm::vec3& scl) {
        scale = scl;
        dirty = true;
    }

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getRotation() const { return rotation; }
    glm::vec3 getScale() const { return scale; }

    const glm::mat4& getModelMatrix() {
        updateModelMatrix();
        return modelMatrix;
    }
};
