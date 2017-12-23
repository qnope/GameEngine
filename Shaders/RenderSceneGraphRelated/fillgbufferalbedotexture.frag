#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(early_fragment_tests) in;

layout(location = 0) in struct FS_IN {
        vec3 position;
        vec3 tangent;
        vec3 normal;
        vec2 textureCoordinate;
}fs_in;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outTangent;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec2 outRoughnessMetalness;

mat3 computeTBN() {
    return mat3(normalize(fs_in.tangent), normalize(cross(fs_in.normal, fs_in.tangent)), normalize(fs_in.normal));
}

vec3 computeNormal(in const mat3 TBN, in const vec3 normalSpace) {
    return normalize(TBN * normalSpace);
}

vec3 computeTangent() {
    return normalize(fs_in.tangent - dot(outNormal.xyz, fs_in.tangent) * outNormal.xyz);
}

vec3 computeAlbedo();
vec3 getNormalInNormalSpace();
float getRoughness();
float getMetalness();

void main() {
        const vec3 normalSpace = getNormalInNormalSpace();
        const mat3 TBN = computeTBN();

        outAlbedo = vec4(computeAlbedo(), 1.0);
        outRoughnessMetalness = vec2(getRoughness(), getMetalness());

        // Gram Schmidt
        outNormal = vec4(computeNormal(TBN, normalSpace), 0.0);
        outTangent = vec4(computeTangent(), 0.0);
}

layout(set = 1, binding = 0) uniform sampler2D albedoTexture;

vec3 computeAlbedo() {
    return pow(textureLod(albedoTexture, fs_in.textureCoordinate, textureQueryLod(albedoTexture, fs_in.textureCoordinate).x).rgb, vec3(2.2));
}

vec3 getNormalInNormalSpace() {
    return vec3(0.0, 0.0, 1.0);
}

float getRoughness() {
    return 1.0;
}

float getMetalness() {
    return 0.0;
}
