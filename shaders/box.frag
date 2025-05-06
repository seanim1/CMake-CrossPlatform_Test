#version 450
#include "Global.comp"
#include "SDF.comp"
layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 camPos;
    float elapsedTime;
    vec3 camDir;
} ubo;

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .001

// Rotation matrix around Y axis
mat3 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        c, 0, -s,
        0, 1,  0,
        s, 0,  c
    );
}
mat3 rotateX(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat3(
        1.0, 0.0, 0.0,
        0.0,   c,  -s,
        0.0,   s,   c
    );
}

// Main raymarch function
float map(vec3 p)
{
    vec3 position = vec3(ubo.model[3]);
    float scaleX = length(ubo.model[0].xyz);
    float scaleY = length(ubo.model[1].xyz);
    float scaleZ = length(ubo.model[2].xyz);
    vec3 scale = vec3(scaleX, scaleY, scaleZ);
    mat3 rotation;
    rotation[0] = ubo.model[0].xyz / scaleX;
    rotation[1] = ubo.model[1].xyz / scaleY;
    rotation[2] = ubo.model[2].xyz / scaleZ;


    // Apply inverse model matrix manually:
    // 1. Translate world position into object space
    p -= position; // Translation

    // 2. Apply inverse rotation
    p = rotation * p; // Rotation over time

    // 3. Apply inverse scale
    p /= 2.0; // Uniform scaling

    return sdTorus(p, vec2(0.7, 0.3)); // Major/minor radius
}

// Raymarching loop
float raymarch(vec3 ro, vec3 rd)
{
    float t = 0.0;
    for (int i = 0; i < 128; i++) {
        vec3 p = ro + t * rd;
        float d = map(p);
        if (d < 0.001) return t;
        if (t > 100.0) break;
        t += d;
    }
    return -1.0;
}

void main() {
    //debugPrintfEXT("uv: %v2f, frag: %v2f, scrn: %v2i\n", uv, gl_FragCoord.xy, SCREEN_DIM);
    //debugPrintfEXT("time: %f\n", ubo.elapsedTime);
    debugPrintfEXT("ubo.camPos: %v3f\n", ubo.camPos);
    vec2 uv = (gl_FragCoord.xy - 0.5 * SCREEN_DIM) / SCREEN_DIM.y;

    vec3 ro = ubo.camPos;
    vec3 rd = normalize(vec3(uv, 1.0));

    float t = raymarch(ro, rd);
    if (t > 0.0) {
        //vec3 col = shade(ro, rd, t);
        vec3 col = vec3(t);
        outColor = vec4(col, 1.0);
    } else {
        outColor = vec4(0.97, 0.91, 0.61 * cos(ubo.elapsedTime), 1.0); // Background
    }

}
