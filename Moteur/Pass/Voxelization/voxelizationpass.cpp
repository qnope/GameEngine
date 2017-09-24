#include "voxelizationpass.h"

VoxelizationPass::VoxelizationPass(Device & device, uint32_t resolution, uint32_t clipMapNumber, SceneGraph & sceneGraph, const Buffer & modelMatricesBuffer, const Buffer & indirectBuffer, ImageFactory & imageFactory, BufferFactory & bufferFactory) :
	mDevice(device),
	mQueryPoolTimeStamp(device, 4),
	mSceneGraph(sceneGraph),
	mCubeVoxelizationInfoStagingBuffer(bufferFactory.createEmptyBuffer(sizeof(CubeVoxelizationInfo), vk::BufferUsageFlagBits::eTransferSrc, false)),
	mCubeVoxelizationInfoBuffer(bufferFactory.createEmptyBuffer(sizeof(CubeVoxelizationInfo), vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, true)),
	mResolution(resolution),
	mClipMapNumber(clipMapNumber)
{
	std::tie(mRenderPass, std::ignore) = RenderPassBuilder::voxelizationPass(device, resolution, clipMapNumber, true, sceneGraph, mPipelineLayout, mCubeVoxelizationInfoDescriptorSet, mCubeVoxelizationInfoBuffer, modelMatricesBuffer, indirectBuffer, mVoxelGrid, imageFactory);
	mFramebuffer = FramebufferBuilder::emptyFrameBuffer(device, vk::Extent2D{ resolution, resolution }, mRenderPass);

	auto ubo = vk::DescriptorBufferInfo{ mCubeVoxelizationInfoBuffer, 0, VK_WHOLE_SIZE };
	auto sampler = vk::DescriptorImageInfo{ mDownsamplerSampler, *mVoxelGrid, vk::ImageLayout::eGeneral };
	auto img = vk::DescriptorImageInfo{ vk::Sampler(), *mVoxelGrid, vk::ImageLayout::eGeneral };

	auto writeUbo = StructHelper::writeDescriptorSet(mDownsamplerDescriptorSet, 0, &ubo, vk::DescriptorType::eUniformBuffer);
	auto writeSampler = StructHelper::writeDescriptorSet(mDownsamplerDescriptorSet, 1, &sampler, vk::DescriptorType::eCombinedImageSampler);
	auto writeImg = StructHelper::writeDescriptorSet(mDownsamplerDescriptorSet, 2, &img, vk::DescriptorType::eStorageImage);

	device->updateDescriptorSets(std::vector<vk::WriteDescriptorSet>{ {writeUbo, writeSampler, writeImg} }, {});
}

void VoxelizationPass::prepare() {
	Cube cube{ mSceneGraph.getAABB() };
	Camera camera{ mSceneGraph.getCamera() };

	CubeVoxelizationInfo info;
	info.clipMapNumber = mClipMapNumber;
	info.voxelGridResolution = mResolution;

	float diagonal = cube.diagonal * 2;

	for (int i = mClipMapNumber - 1; i > -1; --i) {
		glm::vec3 center = camera.position;

		info.cubeCenterDiagonal[i] = glm::vec4(center, diagonal);

		diagonal /= 2.0f;
	}

	const glm::vec3 left1 = info.cubeCenterDiagonal[0].xyz - info.cubeCenterDiagonal[0].w / 2.0f;
	const glm::vec3 left2 = info.cubeCenterDiagonal[0].xyz - info.cubeCenterDiagonal[1].w / 2.0f;
	
	std::cout << "src = " << glm::ivec3(glm::ceil(128.0f * glm::fract(left1 / info.cubeCenterDiagonal[0].w)));
	std::cout << "src = " << (2 * glm::ivec3(glm::ceil(128.0f * glm::fract(left1 / info.cubeCenterDiagonal[1].w)))) % 128;
	std::cout << "dst = " << glm::ivec3(glm::ceil(128.0f * glm::fract(left1 / info.cubeCenterDiagonal[1].w))) << std::endl;

	mCubeVoxelizationInfoStagingBuffer.resetOffset();
	mCubeVoxelizationInfoStagingBuffer.push_data(info);
}

void VoxelizationPass::execute(vk::CommandBuffer cmd)
{	
	mQueryPoolTimeStamp.reset(cmd);
	mQueryPoolTimeStamp.begin(cmd, "Complete Voxelization");

	mQueryPoolTimeStamp.begin(cmd, "Buffer Info Transfer and Clear Grid");
		StructHelper::transfer(mCubeVoxelizationInfoStagingBuffer, mCubeVoxelizationInfoBuffer, cmd); // barrier done  in the renderPass

		auto preClear = MemoryBarrier::image(*mVoxelGrid, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, ImageSubresource::rangeColor(0, 1), vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite);
		auto postClear = MemoryBarrier::image(*mVoxelGrid, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral, ImageSubresource::rangeColor(0, 1), vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderWrite);

		MemoryBarrier::barrier(preClear, cmd, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer);
		cmd.clearColorImage(*mVoxelGrid, vk::ImageLayout::eTransferDstOptimal, Clear::black(), ImageSubresource::rangeColor(0, 1));
		MemoryBarrier::barrier(postClear, cmd, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader);
	mQueryPoolTimeStamp.end(cmd);

	mQueryPoolTimeStamp.begin(cmd, "Voxelization of the scene");
		mRenderPass.execute(cmd, mFramebuffer, vk::Extent2D{ mResolution, mResolution });
	mQueryPoolTimeStamp.end(cmd);

	auto preBuildMipmap = MemoryBarrier::image(*mVoxelGrid, vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral, ImageSubresource::rangeColor(0, 1), vk::AccessFlagBits::eShaderWrite, vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite);
	
	mQueryPoolTimeStamp.begin(cmd, "Mipmap Creation");
		MemoryBarrier::barrier(preBuildMipmap, cmd, vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eComputeShader);

		cmd.bindPipeline(vk::PipelineBindPoint::eCompute, mDownsamplerPipeline);
		cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, mDownsamplerPipelineLayout, 0, mDownsamplerDescriptorSet, nullptr);

		for (int i = 1; i < mClipMapNumber; ++i) {

			auto dispatch = mResolution / 8 / 2;

			cmd.pushConstants(mDownsamplerPipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, sizeof(int), &i);
			cmd.dispatch(dispatch, dispatch, dispatch);

			vk::ImageLayout outLayout = (i == mClipMapNumber - 1) ? vk::ImageLayout::eShaderReadOnlyOptimal : vk::ImageLayout::eGeneral;
			vk::AccessFlags outAccess = (i == mClipMapNumber - 1) ? vk::AccessFlagBits::eShaderRead : vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;

			auto postDispatch = MemoryBarrier::image(*mVoxelGrid, vk::ImageLayout::eGeneral, outLayout, ImageSubresource::rangeColor(0, 1), vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite, outAccess);
			MemoryBarrier::barrier(postDispatch, cmd, vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader);

		}

	mQueryPoolTimeStamp.end(cmd);
	mQueryPoolTimeStamp.end(cmd);
}

std::vector<Profiling> VoxelizationPass::getProfiling()
{
	return mQueryPoolTimeStamp.getNanoSeconds();
}

const CombinedImage & VoxelizationPass::getVoxelGrid() const
{
	return *mVoxelGrid;
}

const Buffer & VoxelizationPass::getCubeVoxelizationInfoBuffer() const
{
	return mCubeVoxelizationInfoBuffer;
}

uint32_t VoxelizationPass::getVoxelGridResolution() const
{
	return mResolution;
}

