#version 450 core
#extension GL_ARB_separate_shader_objects : enable

/** Inspired from https://github.com/Friduric/voxel-cone-tracing/blob/master/Shaders/Voxelization/voxelization.geom **/

layout(triangles) in;

#define MAX_CLIPMAP 10

layout(triangle_strip, max_vertices = 3 * MAX_CLIPMAP) out;

layout(set = 0, binding = 0, std140) uniform CubeVoxelizationInfo {
	uint clipMapNumber;
	uint voxelGridResolution;
	uint p1, p2;
	vec4 cubeCenterDiagonal[MAX_CLIPMAP]; // xyz center, w diagonal length
};

in gl_PerVertex
{
  vec4 gl_Position;
} gl_in[];

layout(location = 0) in struct GS_IN {
	vec3 worldPosition;
}gs_in[];

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out struct GS_OUT {
	vec3 worldPosition;
	float indexClipMap;
}gs_out;

vec3 projectPositionIntoMinusOneOne(in const vec3 currentPosition, in const uint indexClipMap) {
	const float semiDiagonal = cubeCenterDiagonal[indexClipMap].w / 2.0;
	return (currentPosition - cubeCenterDiagonal[indexClipMap].xyz) / semiDiagonal;
}

vec2[3] computeTriangle(in const vec3 projectedPosition[3], in const uint x, in const uint y) {
	const float offset = 4.0 / voxelGridResolution;

	vec2 t[3] = vec2[3](vec2(projectedPosition[0][x], projectedPosition[0][y]),
						vec2(projectedPosition[1][x], projectedPosition[1][y]),
						vec2(projectedPosition[2][x], projectedPosition[2][y]));
		
	const vec2 v01 = normalize(t[1] - t[0]);
	const vec2 v02 = normalize(t[2] - t[0]);
	const vec2 v12 = normalize(t[2] - t[1]);
		
	t[0] += offset * (-v01 - v02);
	t[1] += offset * (v01 - v12);
	t[2] += offset * (v02 + v12);
	
	return t;
}

void emitVertices(in const vec3 projectedPosition[3], in const uint x, in const uint y, in const uint indexClipMap) {
	const vec2 triangle[3] = computeTriangle(projectedPosition, x, y);
	
	gs_out.worldPosition = gs_in[0].worldPosition;
	gs_out.indexClipMap = float(indexClipMap);
	gl_Position = vec4(triangle[0], 0, 1);
	EmitVertex();
		
	gs_out.worldPosition = gs_in[1].worldPosition;
	gs_out.indexClipMap = float(indexClipMap);
	gl_Position = vec4(triangle[1], 0, 1);
	EmitVertex();
		
	gs_out.worldPosition = gs_in[2].worldPosition;
	gs_out.indexClipMap = float(indexClipMap);
	gl_Position = vec4(triangle[2], 0, 1);
	EmitVertex();
}

void main(){
	for(int i = 0; i < clipMapNumber; ++i) {
		const vec3 projectedPosition[3] = vec3[3](projectPositionIntoMinusOneOne(gs_in[0].worldPosition, i),
												  projectPositionIntoMinusOneOne(gs_in[1].worldPosition, i),
												  projectPositionIntoMinusOneOne(gs_in[2].worldPosition, i));
	
		const vec3 p1 = projectedPosition[1] - projectedPosition[0];
		const vec3 p2 = projectedPosition[2] - projectedPosition[0];
		const vec3 p = abs(cross(p1, p2));
	
		if(p.z > p.x && p.z > p.y)	
			emitVertices(projectedPosition, 0, 1, i);
		
		else if (p.x > p.y && p.x > p.z)
			emitVertices(projectedPosition, 1, 2, i);
		
		else
			emitVertices(projectedPosition, 0, 2, i);
		EndPrimitive();
	}
}