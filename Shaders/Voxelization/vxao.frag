#version 450 core

/** Inspired from https://github.com/Friduric/voxel-cone-tracing/blob/master/Shaders/Voxel%20Cone%20Tracing/voxel_cone_tracing.frag and http://machinezero.github.io/post.html#b **/

layout(set = 0, binding = 0, std140) uniform Matrices {
	mat4 projectionMatrix;
	mat4 cameraMatrix;
	vec4 cameraPosition;
};

#define MAX_CLIPMAP 10

layout(set = 0, binding = 1, std140) uniform CubeVoxelizationInfo {
	uint clipMapNumber;
	uint voxelGridResolution;
	uint p1, p2;
	vec4 cubeCenterDiagonal[MAX_CLIPMAP]; // xyz center, w diagonal length
};

layout(set = 0, binding = 2, std140) uniform VXAOParameter {
	float offsetCone;
	int numberCones;
	float overSampling;
	float distanceAttenuation;
	float force;
	float maxDistanceInPercent;
};

layout(set = 0, binding = 3) uniform sampler2D depthMap;

layout(set = 0, binding = 4) uniform sampler2D tangentMap;
layout(set = 0, binding = 5) uniform sampler2D normalMap;

layout(set = 0, binding = 6) uniform sampler3D voxelGrid;

layout(location = 0) in struct FS_IN {
	vec2 uv;
}fs_in;

layout(location = 0) out float ambientOcclusion;

// Hammersley function (return random low-discrepency points)
vec2 Hammersley(uint i)
{
  return vec2(
    float(i) / float(numberCones),
    float(bitfieldReverse(i)) * 2.3283064365386963e-10
  );
}

vec3 getPosSampling(in const vec3 worldPosition, in const uint level) {
	const float diagonal = cubeCenterDiagonal[level].w;
	return fract(worldPosition / diagonal);
}

float sampleAnisotropic(in const vec3 worldPosition, in const uint level, in const vec3 weight) {
	const vec3 posSampling = getPosSampling(worldPosition, level);
	const float factorDirection = 1.0 / 3.0;

	vec3 pos = (posSampling * float(voxelGridResolution) + vec3(1.0)) / float(voxelGridResolution + 2);
	
	pos.y += level;
	pos.y /= float(clipMapNumber);
	pos.x *= factorDirection;
	
	vec3 result = vec3(0.0);
	result.x += texture(voxelGrid, pos).x;
	result.y += texture(voxelGrid, pos + vec3(factorDirection, 0, 0)).x;
	result.z += texture(voxelGrid, pos + vec3(2.0 * factorDirection, 0, 0)).x;
	return clamp(dot(result, weight), 0, 1);
}

float sampleAnisotropicLinearly(in const vec3 worldPosition, in const vec3 dir, in const float level) {
	const vec3 weight = dir * dir;
	
	const uint low = uint(floor(level));
	const uint high = uint(ceil(level));
	
	const float lowSample = sampleAnisotropic(worldPosition, low, weight);
	
	if(low == high)
		return lowSample;
	
	const float highSample = sampleAnisotropic(worldPosition, high, weight);
	
	return mix(lowSample, highSample, fract(level));
}

vec3 alignVToNormal(in const vec3 v, in const vec3 tangeant, in const vec3 normal) {
	mat3 TBN = mat3(tangeant, cross(normal, tangeant), normal);
	
	return normalize(TBN * v);
}

float getMinLevel(in const vec3 worldPosition) {
	const vec3 v = abs(cubeCenterDiagonal[0].xyz - worldPosition);
	const float d = max(max(v.x, v.y), v.z);
	const float radius = cubeCenterDiagonal[0].w / 2.0;
	return max(-1.0, log2(d / radius)) + 1.0; // Transition to the next level
}

float coneTracing(in vec3 start, in const vec3 direction, in const float tanHalfAperture, in const float maxDistance, in const float startLevel) {
	float occlusion = 0.0;
	
	const float coneApertureCoeff = 2.0 * tanHalfAperture;
	const float voxelSizeL0 = cubeCenterDiagonal[0].w / voxelGridResolution;
	float voxelSize = voxelSizeL0 * exp2(startLevel);
	const vec3 offset = voxelSize * (offsetCone * direction);
	float dist = 0.0;
	start += offset;
	
	float distToIncrement = voxelSize;
	
	while(occlusion < 1.0 && dist < maxDistance) {
		const float currentLevel = getMinLevel(start);
				
		const float diameter = max(dist * coneApertureCoeff, voxelSizeL0);
				
		//const float level = min(max(max(log2(diameter / voxelSizeL0), currentLevel), startLevel), clipMapNumber - 1);
		const float level = min(max(log2(diameter / voxelSizeL0), currentLevel), clipMapNumber - 1);
		voxelSize = voxelSizeL0 * exp2(level);
		const float voxel = sampleAnisotropicLinearly(start, direction, level);
		
		float opacityCorrection = distToIncrement / voxelSize;
		
		occlusion += force * (1.0 - occlusion) * (1.0 - pow(1.0 - voxel, opacityCorrection)) / (1.0 + distanceAttenuation * (dist / voxelSize));
		
		distToIncrement = max(diameter, voxelSizeL0) / overSampling;
		dist += distToIncrement;
		start += distToIncrement * direction;
	}
	
	return occlusion;
}

void main() {
	const float z = texture(depthMap, fs_in.uv).x;
	vec4 start = inverse(projectionMatrix) * vec4(fs_in.uv * 2.0 - 1.0, z, 1.0);
	start = inverse(cameraMatrix) * (start / start.w);
	const vec3 tangent = texture(tangentMap, fs_in.uv).xyz;
	const vec3 normal = texture(normalMap, fs_in.uv).xyz;
	
	const float tanHalfAperture = sqrt(2.0 / float(numberCones));
	const float startLevel = getMinLevel(start.xyz);
	const float maxDistance = cubeCenterDiagonal[clipMapNumber - 1].w * maxDistanceInPercent;
	float ao = 0.0;
	
	if(dot(normal, normal) > 0.2) {
		for(uint i = 0; i < numberCones; ++i) {
			const vec2 Xi = Hammersley(i);
			const float cosTheta = 1.0 - Xi.x;
			const float sinTheta = 1.0 - cosTheta * cosTheta;
			const float phi = 6.28 * Xi.y;
			
			const vec3 dir = alignVToNormal(vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta), tangent, normal);
			
			ao += cosTheta * coneTracing(start.xyz, dir, tanHalfAperture, maxDistance, startLevel);
		}
	}
	
	ambientOcclusion = 1.0 - 2.0 * ao / float(numberCones);
}