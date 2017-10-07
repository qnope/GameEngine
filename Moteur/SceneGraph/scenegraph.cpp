#include "scenegraph.h"

#include "Material/albedocolormaterialmanager.h"
#include "Material/albedotexturematerialmanager.h"
#include "Material/pbrnormaltexturematerialmanager.h"

SceneGraph::SceneGraph(Device & device) :
	mImageFactory(device),
	mBufferFactory(device),
	mMeshManager(mBufferFactory),
	mRootNode(std::make_shared<Node>(mMeshManager, mMaterialsManager))
{
	mMaterialsManager.addKindMaterialManager(std::make_unique<AlbedoColorMaterialManager>(device, mBufferFactory));
	mMaterialsManager.addKindMaterialManager(std::make_unique<AlbedoTextureMaterialManager>(device, mImageFactory));
	mMaterialsManager.addKindMaterialManager(std::make_unique<PBRNormalTextureMaterialManager>(device, mImageFactory));
}

std::shared_ptr<Node> SceneGraph::getRootNode()
{
	return mRootNode;
}

uint32_t SceneGraph::getNumberInstances() const
{
	return mRootNode->getNumberInstances();
}

uint32_t SceneGraph::getNumberDrawCalls() const
{
	return mRootNode->getNumberDrawCalls();
}

void SceneGraph::computeMatrices(vector_view<glm::mat4> &matriceBufferView)
{
	mRootNode->computeMatrices(glm::mat4(1.0f), matriceBufferView);
}

void SceneGraph::buildIndirectCommands(vector_view<vk::DrawIndexedIndirectCommand>& indirectCommandBufferView)
{
	uint32_t unused = 0;
	mRootNode->buildIndirectCommands(indirectCommandBufferView, unused);
}

void SceneGraph::prepareWithoutMaterials(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout)
{
	mDrawers.clear();
	mEnableMaterials = false;

	mNonMaterialPipeline = pipeline;
	mNonMaterialPipelineLayout = pipelineLayout;

	mRootNode->draw(mDrawers, false);
}

void SceneGraph::prepareWithMaterials(const std::vector<vk::Pipeline>& pipelines, const std::vector<vk::PipelineLayout>& pipelineLayouts)
{
	mDrawers.clear();
	mEnableMaterials = true;
	
	mMaterialsManager.fillPipelines(pipelines);
	mMaterialsManager.fillPipelineLayouts(pipelineLayouts);

	mRootNode->draw(mDrawers, true);
}

void SceneGraph::compile(vk::CommandBuffer cmd, uint32_t materialSetNumber, vk::Buffer indirectBuffer)
{
	for (auto i(0u); i < mDrawers.size(); ++i) {
		cmd.bindVertexBuffers(0, mDrawers[i].vbo, { 0 });
		cmd.bindIndexBuffer(mDrawers[i].ibo, 0, vk::IndexType::eUint32);

		if (mEnableMaterials == true) {
			cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mDrawers[i].pipeline);

			cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mDrawers[i].pipelineLayout, materialSetNumber, mDrawers[i].materialSet,
				((mDrawers[i].offsetMaterialUbo != ~(0u)) ? mDrawers[i].offsetMaterialUbo : vk::ArrayProxy<const uint32_t>{ nullptr }));
		}

		cmd.drawIndexedIndirect(indirectBuffer, (vk::DeviceSize)(sizeof(vk::DrawIndexedIndirectCommand) * i), 1, sizeof(vk::DrawIndexedIndirectCommand));
	}
}

const MaterialsManager & SceneGraph::getMaterialsManager() const
{
	return mMaterialsManager;
}

AABB SceneGraph::getAABB() const
{
	return mRootNode->getAABB();
}

void SceneGraph::setCamera(Camera camera)
{
	mCamera = camera;
}

Camera SceneGraph::getCamera() const
{
	return mCamera;
}
