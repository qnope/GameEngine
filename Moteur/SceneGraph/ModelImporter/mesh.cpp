#include "mesh.h"

using namespace glm;

Mesh::Mesh(aiMesh const *mesh) {
	for (unsigned i(0); i < mesh->mNumVertices; ++i) {
		Vertex v;

		v.position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		v.tangeant = vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		v.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->HasTextureCoords(0))
			v.textureCoordinate = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

		vertices.emplace_back(v);
		aabb << v;
	}

	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
		for (unsigned j = 0; j < 3; ++j)
			indices.emplace_back(mesh->mFaces[i].mIndices[j]);

	materialIndex = mesh->mMaterialIndex;
	numberVertices = mesh->mNumVertices;
	numberIndices = mesh->mNumFaces * 3;
}

AABB & operator<<(AABB & aabb, const Vertex & v)
{
	aabb.min = glm::min(aabb.min, v.position);
	aabb.max = glm::max(aabb.max, v.position);
	
	return aabb;
}


