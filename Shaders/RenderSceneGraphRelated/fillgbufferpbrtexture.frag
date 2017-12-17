#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(early_fragment_tests) in;

layout(set = 1, binding = 0) uniform sampler2D albedoTexture;
layout(set = 1, binding = 1) uniform sampler2D normalTexture;
layout(set = 1, binding = 2) uniform sampler2D roughnessTexture;
layout(set = 1, binding = 3) uniform sampler2D metallicTexture;

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

#define getTextureLod(sampler) textureLod(sampler, fs_in.textureCoordinate, textureQueryLod(sampler, fs_in.textureCoordinate).x)

void main() {
	outAlbedo = pow(getTextureLod(albedoTexture), vec4(2.2));
	outRoughnessMetalness = vec2(getTextureLod(roughnessTexture).x, getTextureLod(metallicTexture).x);
	
	const vec3 normalSpace = normalize(getTextureLod(normalTexture).xyz * 2.0 - 1.0);
	
	const mat3 TBN = mat3(normalize(fs_in.tangent), normalize(cross(fs_in.normal, fs_in.tangent)), normalize(fs_in.normal));
	
	// Graham Schmidt
	outNormal = vec4(normalize(TBN * normalSpace), 0.0);
	outTangent = vec4(normalize(fs_in.tangent - dot(outNormal.xyz, fs_in.tangent) * outNormal.xyz), 0.0);
}
