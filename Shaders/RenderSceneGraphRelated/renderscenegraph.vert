#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(set = 0, binding = 0, std140) uniform Matrices {
	mat4 projectionMatrix;
	mat4 cameraMatrix;
	vec4 cameraPosition;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inTangent;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTextureCoordinate;
layout(location = 4) in mat4 inModelMatrix;

layout(location = 0) out struct VS_OUT {
	vec3 position;	
	vec3 tangent;
	vec3 normal;
	vec2 textureCoordinate;
}vs_out;

void main() {
    const vec4 worldPosition = inModelMatrix *  vec4(inPosition, 1.0);
    gl_Position = projectionMatrix * cameraMatrix * worldPosition;
    mat3 normalMatrix = transpose(inverse(mat3(inModelMatrix)));
    vs_out.position = worldPosition.xyz;
    vs_out.tangent = normalMatrix * inTangent;
    vs_out.normal = normalMatrix * inNormal;
    vs_out.textureCoordinate = inTextureCoordinate;
}
