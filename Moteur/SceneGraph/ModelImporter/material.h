#pragma once

#include <assimp/material.h>
#include "../../Transfer/imagefactory.h"
#include "../../Tools/glm.h"

struct Material {
	Material(aiMaterial *mtl, std::string const &globalPath);

	std::string albedoTexturePath;
	glm::vec3 albedoColor;
	bool useAlbedoTexture;
};
