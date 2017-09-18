#include "meshmanager.h"
#include "../Tools/stream.h"

static int isBufferAvailable(const std::vector<std::tuple<uint32_t, Buffer>> &buffers, vk::DeviceSize size) {
	for (auto i = 0u; i < buffers.size(); ++i)
		if (std::get<0>(buffers[i]) + size < std::get<1>(buffers[i]).getSize())
			return i;
	return -1;
}

MeshManager::MeshManager(BufferFactory &bufferFactory) :
	mBufferFactory(bufferFactory),
	mStagingBuffer(bufferFactory.createEmptyBuffer(1 << 23, vk::BufferUsageFlagBits::eTransferSrc, false)) {

}

DrawCmd MeshManager::addMesh(const std::vector<Mesh> &meshes)
{
	DrawCmd drawCmd;
	for (const auto &m : meshes) {
		std::size_t vboSize = m.numberVertices * sizeof(Vertex);
		std::size_t iboSize = m.numberIndices * sizeof(uint32_t);
		std::size_t indexVbo = isBufferAvailable(mVbos, vboSize);
		std::size_t indexIbo = isBufferAvailable(mIbos, iboSize);

		if (indexVbo == -1) {
			mVbos.emplace_back(std::make_tuple(0u, mBufferFactory.createEmptyBuffer(1 << 23, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, true)));
			indexVbo = mVbos.size() - (std::size_t)1;
		}

		if (indexIbo == -1) {
			mIbos.emplace_back(std::make_tuple(0u, mBufferFactory.createEmptyBuffer(1 << 23, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, true)));
			indexIbo = mIbos.size() - (std::size_t)1;
		}

		mStagingBuffer.resetOffset();
		mStagingBuffer.push_data(m.vertices);
		mBufferFactory.transfer(mStagingBuffer, std::get<1>(mVbos[indexVbo]), 0, std::get<0>(mVbos[indexVbo]), vboSize);

		mStagingBuffer.resetOffset();
		mStagingBuffer.push_data(m.indices);
		mBufferFactory.transfer(mStagingBuffer, std::get<1>(mIbos[indexIbo]), 0, std::get<0>(mIbos[indexIbo]), iboSize);

		DrawCmd::_ObjectPart part;
		part.ibo = std::get<1>(mIbos[indexIbo]);
		part.vbo = std::get<1>(mVbos[indexVbo]);
		part.vertexOffset = std::get<0>(mVbos[indexVbo]) / sizeof(Vertex);
		part.firstIndex = std::get<0>(mIbos[indexIbo]) / sizeof(uint32_t);
		part.indexCount = m.numberIndices;
		part.aabb = m.aabb;

		drawCmd.mParts << part;

		std::get<0>(mVbos[indexVbo]) += (uint32_t) vboSize;
		std::get<0>(mIbos[indexIbo]) += (uint32_t) iboSize;
	}
	return drawCmd;
}
