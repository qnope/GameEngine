#pragma once

#include "abstractuniquematerialmanager.h"

class MaterialsManager {
public:
	MaterialsManager() = default;

	void addKindMaterialManager(std::unique_ptr<AbstractUniqueMaterialManager> &&ptr);
	std::vector<AbstractUniqueMaterialManager::MaterialPointer> addMaterials(std::vector<Material> materials);

	std::vector<vk::DescriptorSetLayout> getAllDescriptorSetLayouts() const;

	void getDrawerMaterialValues(Drawer &drawer, const AbstractUniqueMaterialManager::MaterialPointer &ptr) const;

	void fillPipelines(const std::vector<vk::Pipeline> &pipelines);
	void fillPipelineLayouts(const std::vector<vk::PipelineLayout> &pipelineLayouts);

private:
	std::vector<std::unique_ptr<AbstractUniqueMaterialManager>> mMaterialManagers;
};