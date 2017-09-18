#include "fillgbuffersubpass.h"
#include "../../Vulkan/structhelper.h"

FillGBufferSubpass::FillGBufferSubpass(vk::Device device, SceneGraph &sceneGraph, vk::DescriptorSetLayout matrixDescriptorSetLayout, vk::DescriptorSet matrixDescriptorSet, const Buffer &modelMatricesBuffer, const Buffer &indirectBuffer) :
	mDevice(device),
	mSceneGraph(sceneGraph),
	mMatrixDescriptorSet(matrixDescriptorSet),
	mModelMatricesBuffer(modelMatricesBuffer),
	mIndirectBuffer(indirectBuffer)
{
	for (int i = 0; i < 3; ++i)
		addColorAttachment(i);

	setDepthStencilAttachment(3);

	auto albedoColorSetLayout = mSceneGraph.getMaterialManager().getAlbedoColorDescriptorSetLayout();
	auto albedoTextureSetLayout = mSceneGraph.getMaterialManager().getAlbedoTextureDescriptorSetLayout();

	mPipelineLayouts.emplace_back(PipelineLayoutBuilder::build(mDevice, { matrixDescriptorSetLayout, albedoColorSetLayout }));
	mPipelineLayouts.emplace_back(PipelineLayoutBuilder::build(mDevice, { matrixDescriptorSetLayout, albedoTextureSetLayout }));
}

void FillGBufferSubpass::create(vk::RenderPass renderPass, vk::Extent2D extent, uint32_t indexPass)
{
	std::array<std::string, 2> fragmentShaderNames{ {"../Shaders/RenderSceneGraphRelated/fillgbufferalbedocolor_frag.spv", "../Shaders/RenderSceneGraphRelated/fillgbufferalbedotexture_frag.spv"} };

	for (auto i(0u); i < mPipelineLayouts.size(); ++i)
		mPipelines.emplace_back( PipelineBuilder::buildFillGBufferPipeline(mDevice, renderPass, indexPass, extent, mPipelineLayouts[i], fragmentShaderNames[i]));
}

void FillGBufferSubpass::execute(vk::CommandBuffer cmd)
{
	static std::vector<vk::Pipeline> pipelines;
	static std::vector<vk::PipelineLayout> pipelineLayouts;

	pipelines.clear();
	pipelineLayouts.clear();

	for (int i = 0; i < mPipelines.size(); ++i) {
		pipelines << *mPipelines[i];
		pipelineLayouts << mPipelineLayouts[i];
	}

	cmd.bindVertexBuffers(1, (vk::Buffer)mModelMatricesBuffer, { 0u });

	// matrix through pipelineLayout compatibilty
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayouts[0], 0, mMatrixDescriptorSet, {});

	mSceneGraph.prepareWithMaterials(pipelines, pipelineLayouts);
	mSceneGraph.compile(cmd, 1, mIndirectBuffer);
}
