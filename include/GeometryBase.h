
#pragma once
#include "Global.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};

class Geometry {
public:
    virtual ~Geometry() = default;

    virtual const void* getVertexData() const = 0;
    virtual uint32_t getVertexCount() const = 0;
    virtual uint16_t getVertexStride() const = 0;

    virtual const uint32_t* getIndexData() const = 0;
    virtual uint32_t getIndexCount() const = 0;

    virtual const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const = 0;
    virtual const std::vector<VkVertexInputBindingDescription>& getBindingDescriptions() const = 0;
};

