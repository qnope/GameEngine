#pragma once

#include <vector>
#include "../Transfer/bufferfactory.h"
#include "ModelImporter/mesh.h"
#include "../Vulkan/buffer.h"
#include "drawer.h"

class MeshManager {
public:
	MeshManager(BufferFactory &bufferFactory);

	DrawCmd addMesh(const std::vector<Mesh> &meshes);

private:
	BufferFactory &mBufferFactory;

	// uint32_t 
	Buffer mStagingBuffer;
	// the uint32_t size means the total offset used
	std::vector<std::tuple<uint32_t, Buffer>> mIbos;
	std::vector<std::tuple<uint32_t, Buffer>> mVbos;
};
