#include "animation/AnimatedModel.h"

void AnimatedModel::loadAnimatedModel(std::string path)
{
	Assimp::Importer importer;
	importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_OptimizeMeshes);
	scene = importer.GetOrphanedScene();

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	std::cout << directory << std::endl;
	
	rootNode = scene->mRootNode;
	AnimNodeProcess();

	GlobalInverseTransform = AiToGlmMat4(scene->mRootNode->mTransformation);
	GlobalInverseTransform = glm::inverse(GlobalInverseTransform);


	processNode(scene->mRootNode, scene);
	

	

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		for (unsigned int j = 0; j < scene->mMeshes[i]->mNumBones; j++)
		{
			

			std::string b_name = scene->mMeshes[i]->mBones[j]->mName.data;
			glm::mat4 b_mat = glm::transpose(AiToGlmMat4(scene->mMeshes[i]->mBones[j]->mOffsetMatrix));
			Bone bone(i, b_name, b_mat);
			bone.node = FindAiNode(b_name);
			bone.animNode = FindAiNodeAnim(b_name);

			if (bone.animNode == nullptr)
				std::cout << "No Animations were found for " + b_name << std::endl;

			//Finally, we push the Bone into our vector. Yay.
			bones.push_back(bone);
		}
	}

	for (int i = 0; i < bones.size(); i++)
	{
		

		std::string b_name = bones.at(i).name;
		std::string parent_name = FindAiNode(b_name)->mParent->mName.data;
		std::cout << parent_name << std::endl;

		Bone* p_bone = FindBone(parent_name);

		

		if (p_bone == NULL){
			std::cout << "Parent Bone for " << b_name << " does not exist (is nullptr)" << std::endl;
			bones[i].parent_bone = NULL;
		}
		else {
			bones[i].parent_bone = p_bone;
		}

		if (meshes.size() > 0){
			skeleton.Init(bones, GlobalInverseTransform);
		}
		else
		{

			std::cout << "Unable to load mesh: " << Path << std::endl;
		}
	}

	std::cout << "failing near here" << std::endl;
	if (meshes.size() != 0)
	{
		meshes.at(0).modelSkeleton = &skeleton;
	}


}

void AnimatedModel::processNode(aiNode *node, const aiScene *scene)
{
	ai_nodes.push_back(node);
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, i));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

AnimatedMesh AnimatedModel::processMesh(aiMesh *mesh, const aiScene *scene, unsigned int meshIndex)
{
	std::vector<AnimatedVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//vertex
		AnimatedVertex vert;
		vert.position.x = mesh->mVertices[i].x;
		vert.position.y = mesh->mVertices[i].y;
		vert.position.z = mesh->mVertices[i].z;

		if (mesh->HasNormals())
		{
			vert.normal.x = mesh->mNormals[i].x;
			vert.normal.y = mesh->mNormals[i].y;
			vert.normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTangentsAndBitangents())
		{

			vert.tangent.x = mesh->mTangents[i].x;
			vert.tangent.y = mesh->mTangents[i].y;
			vert.tangent.z = mesh->mTangents[i].z;

		}
		else {
			vert.tangent = glm::vec3(0.0f);
		}

		if (mesh->mTextureCoords[0])
		{
			vert.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vert.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vert.TexCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vert);

	}

	//indices 
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int k = 0; k < face.mNumIndices; k++)
		{
			indices.push_back(face.mIndices[k]);
		}
	}

	//material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());


		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		//Assimp throws issues when importing reflection maps correctly, nano AnimatedModel stores 
		//Reflection maps as ambient maps, this will obviously need corrected when PBR is implemented
		std::vector<Texture> reflectionMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "reflection");
		textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());


		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		//std::cout << diffuseMaps[0].t_Path << " diffuse map count: " << diffuseMaps.size() << std::endl;
		//std::cout << diffuseMaps[0].t_Path << " specular map count: " << specularMaps.size() << std::endl;
		//std::cout << diffuseMaps[0].t_Path << " reflection/ambient map count: " << reflectionMaps.size() << std::endl;
		//std::cout << diffuseMaps[0].t_Path << " normal map count: " << normalMaps.size() << std::endl;
	}

	//Bones
	if (mesh->HasBones())
	{
		int WEIGHTS_PER_VERTEX = 4;
		int boneArraysSize = mesh->mNumVertices * WEIGHTS_PER_VERTEX; // 4 weights per vertex
		std::vector<int> boneIds;
		boneIds.resize(boneArraysSize);
		std::vector<float> boneWeights;
		boneWeights.resize(boneArraysSize);
		
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* currentBone = mesh->mBones[i];

			for (int j = 0; j < currentBone->mNumWeights; j++)
			{
				aiVertexWeight weight = currentBone->mWeights[j];
				unsigned int vertexStart = weight.mVertexId * WEIGHTS_PER_VERTEX;

				for (int k = 0; k < WEIGHTS_PER_VERTEX; k++)
				{
					if (boneWeights.at(vertexStart + k) == 0)
					{
						int idLocation = vertexStart + k;
						boneWeights.at(idLocation) = weight.mWeight;
						boneIds.at(idLocation) = i;
						vertices.at(weight.mVertexId).boneIds[k] = i;
						vertices.at(weight.mVertexId).boneWeights[k] = weight.mWeight;
					}
				}
			}
		}

	}

	return AnimatedMesh(vertices, indices, textures);
}

std::vector<Texture> AnimatedModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::cout << &str << std::endl;

		bool shouldSkip = false;


		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].t_Path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				shouldSkip = true;
				break;
			}
		}

		if (!shouldSkip)
		{
			Texture texture;
			std::string textureLoadPath = directory + "/" + str.C_Str();

			texture.t_Id = TextureFromFile(textureLoadPath.c_str(), directory);
			texture.t_Type = typeName;
			std::cout << typeName << std::endl;
			texture.t_Path = str.C_Str();
			std::cout << texture.t_Path << std::endl;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}

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



int AnimatedModel::CommonTextureLoad(const char* path)
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


void AnimatedModel::AnimNodeProcess()
{
	if (scene->mNumAnimations == 0)
		return;

	for (int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
		ai_nodes_anim.push_back(scene->mAnimations[0]->mChannels[i]);

	for (int i = 0; i < scene->mNumAnimations; i++)
	{
		ai_animations.push_back(*scene->mAnimations[i]);
	}

	//We only get data from the first mAnimation because 
	//Assimp crushes all of the animation data into one
	//large sequence of data known as mAnimation.
	//Assimp does not support multiple mAnimations, surprisingly.

	int debug = 1;

}

Bone* AnimatedModel::FindBone(std::string in_name)
{
	for (int i = 0; i < bones.size(); i++)
	{
		if (bones.at(i).name == in_name)
			return &bones.at(i);
	}
	
	//This function simply scans our vector bones and checks if
	//any name matches the name we're looking for, if it doesn't
	//find any, we return nullptr.
	//Keep in mind, the bones vector is empty at the point of writing this,
	//but when this function is called it will already be filled up.
	return NULL;
}


aiNode* AnimatedModel::FindAiNode(std::string name)
{
	for (int i = 0; i < ai_nodes.size(); i++)
	{
		if (ai_nodes.at(i)->mName.data == name)
			return ai_nodes.at(i);
	}
	//This function's purpose is identical, except that instead of Bones,
	//it's looking for an aiNode* inside our ai_nodes vector.
	//This vector has already been filled by our recursiveNodeProcess() function.
	return nullptr;
}

aiNodeAnim* AnimatedModel::FindAiNodeAnim(std::string name)
{
	for (int i = 0; i < ai_nodes_anim.size(); i++)
	{
		if (ai_nodes_anim.at(i)->mNodeName.data == name)
			return ai_nodes_anim.at(i);
	}
	//This function finds the animation with the name we pass in, we called it
	//right after calling our recursiveNodeProcess() function, but this function
	//will only really come into play during the next tutorial, where we cover
	//the actual animation portion of skeletal animation.
	return nullptr;
}

int AnimatedModel::FindBoneIDByName(std::string name)
{
	for (int i = 0; i < bones.size(); i++)
	{
		if (bones.at(i).name == name)
			return i;
	}
	//This function finds the position of a certain bone within our bones vector.
	//This position is equal to the bone's ID, which is vital to determining the
	//rigging of our model within the vertex shader.
	return -1;    //In case we don't find a bone ID, we return -1.
				  //Just to avoid any confusion later on as to whether or not the
				  //ID was found. (It serves the same purpose as returning nullptr).
}

void AnimatedModel::AddAnimation(Animation& in_anim)
{
	animations.push_back(in_anim);
}

Animation* AnimatedModel::FindAnimation(std::string anim_to_find)
{
	for (int i = 0; i < animations.size(); i++)
	{
		if (animations.at(i).name == anim_to_find)
			return &animations.at(i);
	}
	return nullptr;
}

void AnimatedModel::PlayAnimation(Animation& anim, bool loop, bool reset_to_start)
{
	
		std::cout << "attmpting to play model from model class " << std::endl;
		skeleton.PlayAnimation(anim, loop, reset_to_start);
	
}

void AnimatedModel::StopAnimating()
{
	skeleton.StopAnimating();
}