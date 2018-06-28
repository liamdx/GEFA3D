#pragma once
#include "Common.h"
#include "Mesh.h"
class MeshCollider {
public:

	MeshCollider(Mesh mesh);
	b3MeshShape meshShape;
	
	b3Mesh* getBounceMesh(Mesh originalMesh);
	b3MeshShape getMeshShape(Mesh originalMesh);
};
