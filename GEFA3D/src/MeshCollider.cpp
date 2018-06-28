#include "MeshCollider.h"

MeshCollider::MeshCollider(Mesh mesh)
{
	meshShape = getMeshShape(mesh);
}

b3Mesh* MeshCollider::getBounceMesh(Mesh originalMesh) 
{
	unsigned int vertexCount = originalMesh.vertices.size();
	unsigned int indexCount = originalMesh.indices.size();
	unsigned int triangleCount = indexCount / 3;

	b3Vec3* verts = new b3Vec3[vertexCount];
	b3Triangle* triangles = new b3Triangle[triangleCount];

	for (int i = 0; i < vertexCount; i++)
	{
		float vertX = originalMesh.vertices[i].position.x;
		float vertY = originalMesh.vertices[i].position.y;
		float vertZ = originalMesh.vertices[i].position.z;
		verts[i].x = vertX;
		verts[i].y = vertY;
		verts[i].z = vertZ;
	}

	for (int i = 0; i < triangleCount; i++)
	{
		unsigned int i1 = originalMesh.indices[i];
		unsigned int i2 = originalMesh.indices[i + 1];
		unsigned int i3 = originalMesh.indices[i + 2];
		triangles[i].v1 = i1;
		triangles[i].v2 = i2;
		triangles[i].v3 = i3;
		i += 3;
	}

	b3Mesh* finalMesh = new b3Mesh();
	finalMesh->vertices = verts;
	finalMesh->triangles = triangles;

	return finalMesh;
}

b3MeshShape MeshCollider::getMeshShape(Mesh originalMesh)
{
	b3MeshShape meshShape;
	meshShape.m_mesh = getBounceMesh(originalMesh);
	return meshShape;
}