#pragma once

#include <assimp/material.h>
#include "../../Transfer/imagefactory.h"
#include "../../Tools/glm.h"

struct Material {
	Material(aiMaterial *mtl, std::string const &globalPath);

	std::string albedoTexturePath;
	std::string normalTexturePath;
	std::string roughnessTexturePath;
	std::string metallicTexturePath;

	glm::vec3 albedoColor;
	bool useAlbedoTexture = false;
	bool useNormalTexture = false;
	bool useRoughnessTexture = false;
	bool useMetallicTexture = false;

	bool isOnlyAlbedoTexture() const;
	bool isNormalPBRTexture() const;
	bool isOnlyAlbedoColor() const;
};
