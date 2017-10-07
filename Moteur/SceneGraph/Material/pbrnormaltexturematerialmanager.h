#pragma once

#include "abstractuniquematerialmanager.h"

#include "../../Transfer/imagefactory.h"

class PBRNormalTextureMaterialManager : public AbstractUniqueMaterialManager {
public:
	struct Descriptor {
		std::shared_ptr<CombinedImage> albedoTexture;
		std::shared_ptr<CombinedImage> normalTexture;
		std::shared_ptr<CombinedImage> roughnessTexture;
		std::shared_ptr<CombinedImage> metallicTexture;
	};

	PBRNormalTextureMaterialManager(vk::Device device, ImageFactory &imageFactory);

	bool isAccepted(Material material) const override;
	MaterialPointer addMaterial(Material material) override;
	void getDrawerMaterialValues(Drawer &drawer, const MaterialPointer &ptr) const override;

private:
	ImageFactory &mImageFactory;
	std::vector<Descriptor> mValues;
	std::vector<vk::DescriptorSet> mDescriptorSets;
};
