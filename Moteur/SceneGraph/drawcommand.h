#pragma once

#include "../Vulkan/vulkan.h"
#include "ModelImporter/mesh.h"
#include "Material/abstractuniquematerialmanager.h"

// A full object 
struct DrawCmd {
    // Part of one object
    struct _ObjectPart {
        vk::Buffer vbo;
        vk::Buffer ibo;
        uint32_t indexCount;
        uint32_t vertexOffset;
        uint32_t firstIndex;
        AbstractUniqueMaterialManager::MaterialPointer materialPointer;
        AABB aabb;
    };

    uint32_t firstInstance = 0;
    uint32_t instanceCount = 1;

    std::vector<_ObjectPart> mParts;
};