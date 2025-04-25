#version 450

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.45),
    vec2(0.5, 0.45),
    vec2(-0.5, 0.45)
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

void main() {
    float angle = ubo.elapsedTime;
    mat2 rotation = mat2(
        cos(angle), -sin(angle),
        sin(angle),  cos(angle)
    );
    vec2 aspectRatio = vec2(16./9., 1.);
    vec2 rotatedPos = rotation * positions[gl_VertexIndex];
    gl_Position = vec4(rotatedPos / aspectRatio, 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
