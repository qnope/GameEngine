#pragma once

#include "material.h"
#include "mesh.h"

class ModelImporter
{
public:
	ModelImporter(std::string const &path);

public:
	std::vector<Mesh> mMeshes;
	std::vector<Material> mMaterials;
};