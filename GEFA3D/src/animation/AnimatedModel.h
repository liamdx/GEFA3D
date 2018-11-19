#pragma once

#include "Common.h"
#include "Shader.h"
class AnimatedModel
{
public:

	AnimatedModel();
	~AnimatedModel();

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;

		BoneInfo()
		{
			BoneOffset;
			FinalTransformation;
		}
	};

	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		BONE_VB,
		NUM_VBs
	};

	GLuint m_VAO;
	GLuint m_Buffers[5];



	struct MeshEntry {
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = 0;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
		unsigned int NumVertices;
		std::vector<Texture> Textures;
	};

	bool LoadMesh(const std::string &filepath);

	void Draw(Shader shader);

	unsigned int NumBones() const
	{
		return m_NumBones;
	}

	void BoneTransform(float TimeInSeconds, std::vector<glm::mat4> &Transforms);

	float currentAnimationLengthInSeconds;

	std::string directory;
	std::vector<MeshEntry> m_Entries;

	void LoadTextureToMesh(unsigned int meshIndex, Texture texture);

	void SetAnimation(int index)
	{
		if (index >= m_pScene->mNumAnimations)
		{
			return;
		}
		else
		{
			currentAnimation = m_pScene->mAnimations[index];
		}
	}

private:

	#define NUM_BONES_PER_VERTEX 4
	

	struct VertexBoneData
	{
		unsigned int IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);

		}

		void AddBoneData(unsigned int BoneID, float Weight);

	};


	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4 ParentTransform);
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int MeshIndex,
		const aiMesh* paiMesh,
		std::vector<glm::vec3>& Positions,
		std::vector<glm::vec3>& Normals,
		std::vector<glm::vec2>& TexCoords,
		std::vector<VertexBoneData>& Bones,
		std::vector<unsigned int>& Indices);
	void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
	glm::mat4 aiToGlm(aiMatrix4x4 ai_matr);
	std::vector<Texture> getMeshMaterial(const aiMesh * mesh, const aiScene * scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	static int TextureFromFile(char const *path, const std::string& directory);
	int EmbeddedTexture(int index);

	void Clear();

	

	std::vector<Texture> m_Textures;

	std::map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
	unsigned int m_NumBones;
	unsigned int m_NumVertices;
	std::vector<BoneInfo> m_BoneInfo;
	glm::mat4 m_GlobalInverseTransform;
	aiAnimation* currentAnimation;
	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
	
};