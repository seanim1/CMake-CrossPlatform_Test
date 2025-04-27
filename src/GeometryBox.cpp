#include "GeometryBox.h"
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

Box::Box(float width, float height, float depth) {
    buildBox(width, height, depth);

    // Set up binding description
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    bindingDescriptions.push_back(bindingDescription);

    // Set up attribute descriptions
    // Position
    attributeDescriptions.resize(2);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);
    // Normal
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, normal);
}

void Box::buildBox(float width, float height, float depth) {
    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;

    glm::vec3 p[] = {
        {-w, -h, -d}, {w, -h, -d}, {w,  h, -d}, {-w,  h, -d}, // back
        {-w, -h,  d}, {w, -h,  d}, {w,  h,  d}, {-w,  h,  d}  // front
    };

    glm::vec3 n[] = {
        {0, 0, -1}, {0, 0, 1}, {1, 0, 0},
        {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}
    };

    std::array<std::array<int, 6>, 6> faces = { {
        {{0, 1, 2, 2, 3, 0}}, // back
        {{4, 5, 6, 6, 7, 4}}, // front
        {{1, 5, 6, 6, 2, 1}}, // right
        {{4, 0, 3, 3, 7, 4}}, // left
        {{3, 2, 6, 6, 7, 3}}, // top
        {{4, 5, 1, 1, 0, 4}}  // bottom
    } };

    for (int face = 0; face < 6; ++face) {
        for (int i = 0; i < 6; ++i) {
            vertices.push_back({ p[faces[face][i]], n[face] });
            indices.push_back(static_cast<uint32_t>(indices.size()));
        }
    }
}

// -- Interface Implementations --

const void* Box::getVertexData() const {
    return vertices.data();
}

uint32_t Box::getVertexCount() const {
    return (uint32_t) vertices.size();
}

uint16_t Box::getVertexStride() const {
    return sizeof(Vertex);
}

const uint32_t* Box::getIndexData() const {
    return indices.data();
}

uint32_t Box::getIndexCount() const {
    return (uint32_t) indices.size();
}

const std::vector<VkVertexInputAttributeDescription>& Box::getAttributeDescriptions() const {
    return attributeDescriptions;
}

const std::vector<VkVertexInputBindingDescription>& Box::getBindingDescriptions() const {
    return bindingDescriptions;
}
