#pragma once

#include "Global.h"
#include <glm/glm.hpp>

// Source for hsv2rgb() and hsv2rgb_smooth() by Inigo Quilez ( https://www.shadertoy.com/view/MsS3Wc )
glm::vec3 hsv2rgb(glm::vec3 hsv) {
    // parameter 'hsv.x' (hue component) must be a value between (-1 to 1) for continuously varying colors or (0 to 1) for only unique colors
    // hsv.y (saturation) and hsv.z (brightness) should also be a value between (0,1)
    glm::vec3 rgb0 = glm::mod(hsv.x * 6.0f + glm::vec3(0.0f, 4.0f, 2.0f), 6.0f);
    glm::vec3 rgb1 = glm::abs(rgb0 - 3.0f) - 1.0f;
    glm::vec3 rgb = glm::clamp(rgb1, 0.0f, 1.0f);
    //glm::vec3 rgb = glm::clamp(glm::abs(glm::mod(hsv.x * 6.0f + glm::vec3(0.0f, 4.0f, 2.0f), 6.0f) - 3.0f) - 1.0f, 0.0f, 1.0f);
    return hsv.z * glm::mix(glm::vec3(1.0f), rgb, hsv.y);
}
// Smooth HSV to RGB conversion (Domain & Range: 0 to 1)
glm::vec3 hsv2rgb_smooth(glm::vec3 hsv) {
    glm::vec3 rgb = glm::clamp(glm::abs(glm::mod(hsv.x * 6.0f + glm::vec3(0.0f, 4.0f, 2.0f), 6.0f) - 3.0f) - 1.0f, 0.0f, 1.0f);
    rgb = rgb * rgb * (3.0f - 2.0f * rgb); // cubic smoothing	
    return hsv.z * glm::mix(glm::vec3(1.0f), rgb, hsv.y);
}