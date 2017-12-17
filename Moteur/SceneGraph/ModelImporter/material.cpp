#include "material.h"
#include <iostream>

static std::string normalizePathTexture(std::string _path) {
    std::string path = std::move(_path);
    if (path[0] == '.')
        path.erase(0, 1);

    if (path[0] == '/' || path[0] == '\\')
        path.erase(0, 1);

    for (auto &v : path)
        if (v == '\\')
            v = '/';
    return path;
}

Material::Material(aiMaterial *mtl, std::string const &globalPath) {
    aiString texPath;

    if (mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
        albedoTexturePath = globalPath + normalizePathTexture(texPath.C_Str());
        useAlbedoTexture = true;
    }

    else {
        aiColor3D color;

        if (mtl->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
            this->albedoColor = glm::vec3(color.r, color.g, color.b);

        else
            this->albedoColor = glm::vec3(1.0, 1.0, 1.0);
        useAlbedoTexture = false;
    }

    // metallic
    if (mtl->GetTexture(aiTextureType_AMBIENT, 0, &texPath) == AI_SUCCESS) {
        metallicTexturePath = globalPath + normalizePathTexture(texPath.C_Str());
        useMetallicTexture = true;
    }

    // Normals
    if (mtl->GetTexture(aiTextureType_HEIGHT, 0, &texPath) == AI_SUCCESS) {
        normalTexturePath = globalPath + normalizePathTexture(texPath.C_Str());
        useNormalTexture = true;
    }

    // Roughness
    if (mtl->GetTexture(aiTextureType_SHININESS, 0, &texPath) == AI_SUCCESS) {
        roughnessTexturePath = globalPath + normalizePathTexture(texPath.C_Str());
        useRoughnessTexture = true;
    }
}

bool Material::isOnlyAlbedoTexture() const
{
    if (useAlbedoTexture && !useNormalTexture && !useRoughnessTexture && !useMetallicTexture)
        return true;
    return false;
}

bool Material::isNormalPBRTexture() const
{
    if (useAlbedoTexture && useNormalTexture && useRoughnessTexture && useMetallicTexture)
        return true;
    return false;
}

bool Material::isOnlyAlbedoColor() const
{
    if (!useAlbedoTexture && !useNormalTexture && !useRoughnessTexture && !useMetallicTexture)
        return true;
    return false;
}
