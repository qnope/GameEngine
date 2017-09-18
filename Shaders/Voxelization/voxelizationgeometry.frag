#version 450 core
#extension GL_ARB_separate_shader_objects : enable

#define X 0
#define Y 1
#define Z 2

#define MAX_CLIPMAP 10

layout(set = 0, binding = 0, std140) uniform CubeVoxelizationInfo {
	uint clipMapNumber;
	uint voxelGridResolution;
	uint p1, p2;
	vec4 cubeCenterDiagonal[MAX_CLIPMAP]; // xyz center, w diagonal length
};

layout(set = 0, binding = 1, r8) restrict writeonly uniform image3D voxelGrid;

layout(location = 0) in struct FS_IN {
	vec3 worldPosition;
	float indexClipMap;
}fs_in;

void storeOpacity(in const uint direction, in const uint indexClipMap, in const ivec3 voxel, float opacity) {
	const ivec3 coords = voxel + ivec3(1) + int(voxelGridResolution + 2) * ivec3(int(direction), int(indexClipMap), 0);
	
	imageStore(voxelGrid, coords, vec4(opacity));
}

bool insideTheCube(const in vec3 worldPosition, const in uint currentClipMap) {
	const float diagonal = cubeCenterDiagonal[currentClipMap].w;
	const vec3 left = cubeCenterDiagonal[currentClipMap].xyz - diagonal / 2.0;
	const vec3 right = left + diagonal;
	return !(any(greaterThan(worldPosition, right)) || any(lessThan(worldPosition, left)));
}

vec3 getPosSampling(in const vec3 worldPosition, in const uint level) {
	const float diagonal = cubeCenterDiagonal[level].w;
	return (worldPosition - cubeCenterDiagonal[level].xyz +	diagonal / 2.0) / diagonal;
}

void main() 
{
	const uint indexClipMap = uint(floor(fs_in.indexClipMap + 0.5));
	
	if(!insideTheCube(fs_in.worldPosition, indexClipMap))
		return;
		
	const ivec3 voxel = ivec3(getPosSampling(fs_in.worldPosition, indexClipMap) * voxelGridResolution) & ivec3(voxelGridResolution - 1);
	
	for(int i = 0; i < 3; ++i)
		storeOpacity(i, indexClipMap, voxel, 1.0);
}