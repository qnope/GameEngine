#version 450 core

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

layout(set = 0, binding = 2, r8) restrict readonly uniform image3D voxelGrid;

layout(location = 0) in struct FS_IN {
	vec2 uv;
}fs_in;

layout(location = 0) out vec4 outColor;

vec3 createRayDirection() {
	const mat4 invProjectionMatrix = inverse(projectionMatrix);
	vec4 end = invProjectionMatrix * vec4(fs_in.uv * 2.0 - 1.0, 1.0, 1.0);
	//const vec4 start = invMatrix * vec4(fragUv * 2.0 - 1.0, 0.0, 1.0);
	
	end /= end.w;
	
	return normalize( (inverse(cameraMatrix) * end).xyz - cameraPosition.xyz);	
}

void main() {
	bool ok;
	const vec3 origin = cameraPosition.xyz;
	const vec3 dir = createRayDirection();
	const float epsilon = 0.0;
	const vec3 invDir = 1.0 / dir;
//	ivec3 xyz = getFirstXYZ(ok);
	
	const vec3 left = cubeCenterDiagonal[0].xyz - cubeCenterDiagonal[0].w / 2.0;
	const float cellDimension = cubeCenterDiagonal[0].w / voxelGridResolution;
	
	ivec3 exit, cell;
	vec3 delta, nextCrossing;
	ivec3 stepXyz;
	
	vec3 rayOrigCell = origin + epsilon * dir - left;
	cell = clamp(ivec3(floor(rayOrigCell / cellDimension)), ivec3(0), ivec3(voxelGridResolution - 1));
	
	for(int i = 0; i < 3; ++i) {
		if(dir[i] < 0.0) {
			delta[i] = -cellDimension * invDir[i];
			nextCrossing[i] = epsilon + (cell[i] * cellDimension - rayOrigCell[i]) * invDir[i];
			exit[i] = -1;
			stepXyz[i] = -1;
		}
		
		else {
			delta[i] = cellDimension * invDir[i];
			nextCrossing[i] = epsilon + ((cell[i] + 1) * cellDimension - rayOrigCell[i]) * invDir[i];
			exit[i] = int(voxelGridResolution);
			stepXyz[i] = 1;
		}
	}
			
	const uint map[8] = uint[8](2, 1, 2, 1, 2, 2, 0, 0);
		
	while(true) {
		if(imageLoad(voxelGrid, cell).x > 0.5) {
			outColor = vec4(vec3(cell) / voxelGridResolution, 1.0);
			break;
		}
		uint k = (uint(nextCrossing.x < nextCrossing.y) << 2) +
				 (uint(nextCrossing.x < nextCrossing.z) << 1) +
				 (uint(nextCrossing.y < nextCrossing.z));
		uint axis = map[k];
		cell[axis] += stepXyz[axis];
		if(cell[axis] == exit[axis])
			break;
		nextCrossing[axis] += delta[axis];
	}
	
}