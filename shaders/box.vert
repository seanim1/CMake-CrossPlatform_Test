#version 450

layout(constant_id = 0) const int SCREEN_WIDTH = 1;
layout(constant_id = 1) const int SCREEN_HEIGT = 1;

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.45),
    vec2(0.65, 0.65),
    vec2(-0.65, 0.65)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    float elapsedTime;
} ubo;

// Range: same as cos()
float wavy(float x) {
    return sin(x) + 0.5 * sin(2.3 * x + 1.0) + 0.25 * sin(4.7 * x - 0.5);
}

void main() {
    float angle = 0.7 * ubo.elapsedTime;
    float scale = (0.05 * wavy(ubo.elapsedTime + gl_VertexIndex * 3.7) + 1.0);
    vec2 rotating_center = vec2(0., -0.08);
    vec2 translation = vec2(0., -0.2);
    mat2 rotation = mat2(
         cos(angle), -sin(angle),
        sin(angle),  cos(angle)
    );
    vec2 aspectRatio = vec2(SCREEN_WIDTH / float(SCREEN_HEIGT), 1.);
    vec2 rotatedPos = rotation * (positions[gl_VertexIndex] * scale + translation + rotating_center) - rotating_center;
    gl_Position = vec4(rotatedPos / aspectRatio, 0.0, 1.0);

    // Color variation
    float a = 0.9;
    int color_component_index = (int(gl_VertexIndex) + 1) % 3;
    vec3 col = colors[int(gl_VertexIndex)];
    col[color_component_index] = (a * wavy(ubo.elapsedTime + gl_VertexIndex) + a);
    fragColor = col;
}
