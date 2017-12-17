#pragma once

#include "abstractuniquematerialmanager.h"

class AlbedoTextureMaterialManager : public AbstractUniqueMaterialManager {
public:
    struct Descriptor {
        std::shared_ptr<CombinedImage> albedoTexture;
    };

    AlbedoTextureMaterialManager(vk::Device device, ImageFactory &imageFactory);

    bool isAccepted(Material material) const override;
    MaterialPointer addMaterial(Material material) override;
    void getDrawerMaterialValues(Drawer &drawer, const MaterialPointer &ptr) const override;

private:
    ImageFactory &mImageFactory;
    std::vector<Descriptor> mValues;
    std::vector<vk::DescriptorSet> mDescriptorSets;
};