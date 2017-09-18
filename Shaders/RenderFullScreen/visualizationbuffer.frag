#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(early_fragment_tests) in;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) in struct FS_IN {
	vec2 uv;
}fs_in;

layout(push_constant) uniform PUSH_CONSTANT {
	bool isMonochromatic;
	bool needGammaCorrection;
};

layout(location = 0) out vec4 outColor;

void main() {
	if(isMonochromatic)
		outColor = texture(tex, fs_in.uv).rrra;//* color;
	
	else
		outColor = texture(tex, fs_in.uv);
	
	if(needGammaCorrection)
		outColor = pow(outColor, vec4(1.0 / 2.2));
}