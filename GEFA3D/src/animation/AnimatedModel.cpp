#include "AnimatedModel.h"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

void AnimatedModel::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for (unsigned int i = 0; i < 4; i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	
}

AnimatedModel::AnimatedModel()
{
	
	m_NumBones = 0;
	m_pScene = NULL;
}


AnimatedModel::~AnimatedModel()
{
	Clear();
}


void AnimatedModel::Clear()
{

	if (m_Buffers[0] != 0) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
	m_BoneMapping.clear();
	m_BoneInfo.clear();
	m_NumBones = 0;
	m_NumVertices = 0;
}


bool AnimatedModel::LoadMesh(const std::string& filepath)
{
	// Release the previously loaded mesh (if it exists)
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Create the buffers for the vertices attributes
	//find this function in ogl
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	bool Ret = false;
	//replace with old loader stuff
	m_pScene = m_Importer.ReadFile(filepath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices );

	if (m_pScene) {
		directory = filepath.substr(0, filepath.find_last_of('/'));
		currentAnimation = m_pScene->mAnimations[0];
		std::cout << "DEBUG: Directory = " << directory << std::endl;
		m_GlobalInverseTransform = aiToGlm(m_pScene->mRootNode->mTransformation.Inverse());
		Ret = InitFromScene(m_pScene, filepath);
		
		
	}
	else {
		printf("Error parsing '%s': '%s'\n", filepath.c_str(), m_Importer.GetErrorString());
	}

	for (int i = 0; i < 1; i++)
	{/*
		for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
			if (std::string(pAnimation->mChannels[i]->mNodeName.data) == NodeName) {
				const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
				return pNodeAnim;*/
		std::map <std::string, const aiNodeAnim*> cm;
		for (unsigned int j = 0; i < m_pScene->mAnimations[i]->mNumChannels; i++) {

			cm[m_pScene->mAnimations[i]->mChannels[j]->mNodeName.data] = m_pScene->mAnimations[i] ->mChannels[j];

		}

		nodeMappings[m_pScene->mAnimations[i]] = cm;
	}
	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);

	return Ret;
}

glm::mat4 AnimatedModel::aiToGlm(aiMatrix4x4 ai_matr)
{
	glm::mat4 result;
	result[0].x = ai_matr.a1; result[0].y = ai_matr.b1; result[0].z = ai_matr.c1; result[0].w = ai_matr.d1;
	result[1].x = ai_matr.a2; result[1].y = ai_matr.b2; result[1].z = ai_matr.c2; result[1].w = ai_matr.d2;
	result[2].x = ai_matr.a3; result[2].y = ai_matr.b3; result[2].z = ai_matr.c3; result[2].w = ai_matr.d3;
	result[3].x = ai_matr.a4; result[3].y = ai_matr.b4; result[3].z = ai_matr.c4; result[3].w = ai_matr.d4;


	return result;
}


bool AnimatedModel::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> TexCoords;
	std::vector<VertexBoneData> Bones;
	std::vector<unsigned int> Indices;
	std::vector<Texture> textures;

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;


	// Count the number of vertices and indices
	for (unsigned int i = 0; i <pScene->mNumMeshes; i++) {
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;
		m_Entries[i].NumVertices = pScene->mMeshes[i]->mNumVertices;
		m_NumVertices += pScene->mMeshes[i]->mNumVertices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the std::vectors for the vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
		m_Entries[i].Textures = getMeshMaterial(paiMesh, m_pScene);
	}

	/*if (!InitMaterials(pScene, Filename)) {
		return false;
	}*/
	//look at old method of filling vao
	// Generate and populate the buffers with vertex attributes and the indices
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	currentAnimationLengthInSeconds = currentAnimation->mDuration / currentAnimation->mTicksPerSecond;


	return true;
}


void AnimatedModel::InitMesh(unsigned int MeshIndex,
	const aiMesh* paiMesh,
	std::vector<glm::vec3>& Positions,
	std::vector<glm::vec3>& Normals,
	std::vector<glm::vec2>& TexCoords,
	std::vector<VertexBoneData>& Bones,
	std::vector<unsigned int>& Indices)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	// Populate the vertex attribute std::vectors
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}

	if (paiMesh->HasBones())
	{
		LoadBones(MeshIndex, paiMesh, Bones);
	}
	

	// Populate the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}

	


	

}

std::vector<Texture> AnimatedModel::getMeshMaterial(const aiMesh *  mesh, const aiScene * scene)
{
	std::vector<Texture> textures;
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());


		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		//Assimp throws issues when importing reflection maps correctly, nano model stores 
		//Reflection maps as ambient maps, this will obviously need corrected when PBR is implemented
		std::vector<Texture> reflectionMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "reflection");
		textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());


		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		return textures;
	}
	else
	{
		return textures;
	}
}

void AnimatedModel::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
	//read the github example thats exaclty the same
	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
		unsigned int BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);

			
			aiMatrix4x4 AssimpMatrix = pMesh->mBones[i]->mOffsetMatrix;

			m_BoneInfo[BoneIndex].BoneOffset = aiToGlm(AssimpMatrix);
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		std::cout << "num vertices per bone: " << pMesh->mBones[i]->mNumWeights << std::endl;

		for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
						
			unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}

	}
}

std::vector<Texture> AnimatedModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		const char* loadpath = str.C_Str();
		int location;
		if (m_pScene->mTextures != nullptr)
		{
			location = atoi(loadpath);
		}
		else
		{
			location = -1;
		}
		

		

		std::cout << "location = " << location << std::endl;

		if (location >= 0)
		{
			Texture texture;
			texture.t_Id = EmbeddedTexture(location);
			texture.t_Type = "diffuse";
			texture.t_Path = "Embedded";
			textures.push_back(texture);
			m_Textures.push_back(texture);
		}
		else
		{
			std::cout << &str << std::endl;

			bool shouldSkip = false;


			for (unsigned int j = 0; j < m_Textures.size(); j++)
			{
				if (std::strcmp(m_Textures[j].t_Path.data(), str.C_Str()) == 0)
				{
					textures.push_back(m_Textures[j]);
					shouldSkip = true;
					break;
				}
			}

			if (!shouldSkip)
			{
				Texture texture;
				std::cout << "DEBUG: Directory before loading: " << directory << std::endl;
				std::string textureLoadPath = directory + "/" + loadpath;
				std::cout << "DEBUG: Texture loading from: " << textureLoadPath << std::endl;;
				texture.t_Id = TextureFromFile(textureLoadPath.c_str(), directory);
				texture.t_Type = typeName;
				std::cout << typeName << std::endl;
				texture.t_Path = str.C_Str();
				std::cout << texture.t_Path << std::endl;
				textures.push_back(texture);
				m_Textures.push_back(texture);
			}
		}

		/*if (loadpath.find("/") > 1)
		{
			std::size_t pos = loadpath.find_last_of('/');
			std::string finalpath = loadpath.substr(pos);
			std::cout << "loadpath after op: " << finalpath << std::endl;
		}*/
		
		
		
		

	}
	return textures;
}

int AnimatedModel::TextureFromFile(char const *path, const std::string& directory)
{
	unsigned int id;
	std::cout << path << std::endl;
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	std::cout << path << std::endl;

	if (data != nullptr)
	{

		GLenum format;

		if (nrChannels == 1)
		{
			format = GL_RED;
		}

		else if (nrChannels == 3)
		{
			format = GL_RGB;
		}

		else if (nrChannels == 4)
		{
			format = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		return id;
	}
	else
	{
		std::cout << "Failed to load texture " << path << std::endl;
		stbi_image_free(data);
		return NULL;
	}


}

int AnimatedModel::EmbeddedTexture(int index)
{
	unsigned int id;
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;

	auto * data = m_pScene->mTextures[0]->pcData;

	width = m_pScene->mTextures[0]->mWidth;
	height = m_pScene->mTextures[0]->mHeight;
	nrChannels = 4;
	GLenum format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return id;
}

void AnimatedModel::Draw(Shader shader)
{
	glBindVertexArray(m_VAO);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

		for (unsigned int j = 0; j < m_Entries[i].Textures.size(); j++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			if (m_Entries[i].Textures[j].t_Type == "diffuse")
			{
				shader.setInt("mat.diffuse", i);
				glBindTexture(GL_TEXTURE_2D, m_Entries[i].Textures[j].t_Id);
			}
		}

		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_Entries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
			m_Entries[i].BaseVertex);
		
		
			for (unsigned int j = 0; j < m_Entries[i].Textures.size(); j++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		
	}

	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);
}


unsigned int AnimatedModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}


unsigned int AnimatedModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}


unsigned int AnimatedModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}


void AnimatedModel::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	if (Factor > 1.0f || Factor < 0.0f)
	{
		Factor = 0.0f;
	}
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
	//Out = Out.Normalize();
}


void AnimatedModel::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	if (Factor > 1.0f || Factor < 0.0f)
	{
		Factor = 0.0f;
	}
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void AnimatedModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	if (Factor > 1.0f || Factor < 0.0f)
	{
		Factor = 0.0f;
	}
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
	//Out.Normalize();
}


void AnimatedModel::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4 ParentTransform)
{
	std::string NodeName(pNode->mName.data);
	  
	const aiAnimation* pAnimation = currentAnimation;
	
	aiMatrix4x4 NodeTransformation = pNode->mTransformation;

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		aiMatrix4x4 scaling_mat;
		aiMatrix4x4::Scaling(Scaling, scaling_mat);



		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix4x4 rotating_mat = aiMatrix4x4(RotationQ.GetMatrix());
		
		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		aiMatrix4x4 translate_mat;
		aiMatrix4x4::Translation(Translation, translate_mat);
		// Combine the above transformations
		
		NodeTransformation = translate_mat * rotating_mat * scaling_mat;
		
	}

	aiMatrix4x4 GlobalTransformation;
	GlobalTransformation = ParentTransform * NodeTransformation;

	//if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		unsigned int BoneIndex = m_BoneMapping[NodeName];
		//fix this aitoglm
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * aiToGlm(GlobalTransformation) * m_BoneInfo[BoneIndex].BoneOffset;
	}
	
	for (int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}

}

void AnimatedModel::LoadTextureToMesh(unsigned int index, Texture texture)
{
	
	if (texture.t_Type == "diffuse")
	{
		for (int i = 0; i < m_Entries[index].Textures.size(); i++)
		{
			if (m_Entries[index].Textures[i].t_Type == "diffuse")
			{
				m_Entries[index].Textures[i] = texture;
			}
		}
	}

	if (texture.t_Type == "specular")
	{
		for (int i = 0; i < m_Entries[index].Textures.size(); i++)
		{
			if (m_Entries[index].Textures[i].t_Type == "specular")
			{
				m_Entries[index].Textures[i] = texture;
			}
		}
	}

	if (texture.t_Type == "normal")
	{
		for (int i = 0; i < m_Entries[index].Textures.size(); i++)
		{
			if (m_Entries[index].Textures[i].t_Type == "normal")
			{
				m_Entries[index].Textures[i] = texture;
			}
		}
	}

	if (texture.t_Type == "reflection")
	{
		for (int i = 0; i < m_Entries[index].Textures.size(); i++)
		{
			if (m_Entries[index].Textures[i].t_Type == "normal")
			{
				m_Entries[index].Textures[i] = texture;
			}
		}
	}

	
}

void AnimatedModel::BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms)
{
	aiMatrix4x4 identity_matrix;
	//currentAnimation.
	float TicksPerSecond = (float)(currentAnimation->mTicksPerSecond != 0 ? currentAnimation->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)currentAnimation->mDuration);

	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, identity_matrix);

	Transforms.resize(m_NumBones);

	for (int i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
		
	}
}


const aiNodeAnim* AnimatedModel::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{

	//can this be done more efficiently?
	/*for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}*/


	/*for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
		if (std::string(pAnimation->mChannels[i]->mNodeName.data) == NodeName) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
			return pNodeAnim;
		}
	}


*/
	return(nodeMappings[pAnimation][NodeName]);
	return NULL;
}
