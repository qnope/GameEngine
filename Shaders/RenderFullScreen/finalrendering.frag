#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(early_fragment_tests) in;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) in struct FS_IN {
	vec2 uv;
}fs_in;

layout(location = 0) out vec4 outColor;

void main() {
	vec4 finalColor = texture(tex, fs_in.uv) ;//* color;
	
	outColor = pow(finalColor, vec4(1/2.2));
}