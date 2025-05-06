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
    vec3 camDir;
    float elapsedTime;
} ubo;

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .001

float GetDist(vec3 p) {
	vec4 s = vec4(0, 0, 0, 0.5);
    
    float sphereDist =  length(p-s.xyz)-s.w;
    float planeDist = p.y;
    
    float d = sphereDist;
    return d;
}


float RayMarch(vec3 ro, vec3 rd) {
	float dO=0.;
    
    for(int i=0; i<MAX_STEPS; i++) {
    	vec3 p = ro + rd*dO;
        float dS = GetDist(p);
        dO += dS;
        if(dO>MAX_DIST || dS<SURF_DIST) break;
    }
    
    return dO;
}
vec3 GetNormal(vec3 p) {
	float d = GetDist(p);
    vec2 e = vec2(.001, 0);
    
    vec3 n = d - vec3(
        GetDist(p-e.xyy),
        GetDist(p-e.yxy),
        GetDist(p-e.yyx));
    
    return normalize(n);
}

float GetLight(vec3 p) {
    vec3 lightPos = vec3(0, 5, 6);
    vec3 l = normalize(lightPos-p);
    vec3 n = GetNormal(p);
    
    float dif = clamp(dot(n, l), 0., 1.);
    float d = RayMarch(p+n*SURF_DIST*2., l);
    if(d<length(lightPos-p)) dif *= .1;
    
    return dif;
}
void main() {
    //debugPrintfEXT("uv: %v2f, frag: %v2f, scrn: %v2i\n", uv, gl_FragCoord.xy, SCREEN_DIM);
    
    // Step 1: Reconstruct world-space ray
    // Normalized pixel coordinates (from 0 to 1)
    // Inverse matrices
    // Access translation directly
    vec3 translation = vec3(ubo.model[3]);
    vec3 adjustedTranslation = translation * 0.001;
    mat4 adjustedModel = ubo.model;
    adjustedModel[3] = vec4(adjustedTranslation, 1.0);
    mat4 invModel = inverse(adjustedModel);

    mat4 invView = inverse(ubo.view);
    mat4 invProj = inverse(ubo.proj);

    // Reconstruct ray in world space
    vec2 uv = gl_FragCoord.xy / SCREEN_DIM;
    vec2 ndc = uv * 2.0 - 1.0;
    ndc.y = -ndc.y; // Flip Y for Vulkan NDC
    //debugPrintfEXT("uv: %v2f, frag: %v2f, scrn: %v2i\n", uv, gl_FragCoord.xy, SCREEN_DIM);

    vec4 clip = vec4(ndc, -1.0, 1.0); // Ray on near plane
    vec4 viewPos = invProj * clip;
    viewPos /= viewPos.w;

    vec3 rayOrigin = ubo.camPos;
    vec3 rayTarget = (invView * viewPos).xyz;
    vec3 rayDir = normalize(rayTarget - rayOrigin);

    // Transform ray into local box space
    vec3 localRayOrigin = (invModel * vec4(rayOrigin, 1.0)).xyz;
    vec3 localRayDir = normalize((invModel * vec4(rayDir, 0.0)).xyz);

    vec3 objPosition = vec3(ubo.model[3]);

    // Raymarch in world space
    float t = 0.0;
    bool hit = false;
    for (int i = 0; i < 128; ++i) {
        //vec3 p = rayOrigin + t * rayDir;
        vec3 p = localRayOrigin + t * localRayDir;

        // Transform the point into the object's local space inside the SDF
        //vec3 localP = (inverse(ubo.model) * vec4(p, 1.0)).xyz;
        float d = sdTorus(p, vec2(0.002, 0.0001)); // Torus in local space
        if (d < 0.0001) {
            hit = true;
            break;
        }
        t += d;
        if (t > 100.0) break;
    }

    // Output
    if (hit) {
        outColor = vec4(1.0, 0.6, 0.3, 1.0);
    } else {
        outColor = vec4(0.97, 0.91, 0.61, 1.0);
    }


}
