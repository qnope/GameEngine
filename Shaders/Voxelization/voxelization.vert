#version 450 core
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;

layout(location = 1) in mat4 modelMatrice;

layout(location = 0) out struct VS_OUT {
	vec3 worldPosition;
}vs_out;

void main() {
	vs_out.worldPosition = vec3(modelMatrice * vec4(inPosition, 1.0));
}