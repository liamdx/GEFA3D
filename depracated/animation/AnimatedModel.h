#pragma once
#include "Shader.h"

#include "AnimatedMesh.h"

class AnimatedModel {

public:
	AnimatedModel(std::string const &path)
	{
		loadAnimatedModel(path);
		Path = path;
	}
	
	std::vector<Texture> textures_loaded;
	std::vector<AnimatedMesh> meshes;
	std::vector<aiNode*> ai_nodes;
	std::vector<aiNodeAnim*> ai_nodes_anim;
	std::vector<Bone> bones;
	Skeleton skeleton;
	const aiScene* scene;
	aiNode* rootNode;
	std::string directory;
	std::vector<Animation> animations;
	std::vector<aiAnimation> ai_animations;


	glm::mat4 GlobalInverseTransform;

	//Animator animator;

	void Draw(Shader shader, float deltaTime) { updateSkeleton(deltaTime); for (unsigned int i = 0; i < meshes.size(); i++) { meshes[i].Draw(shader); } }
	void TestDraw(Shader shader, float deltaTime) { updateSkeleton(deltaTime); for (unsigned int i = 0; i < meshes.size(); i++) { meshes[i].TestDraw(shader); } }
	static int TextureFromFile(char const *path, const std::string& directory);
	static int CommonTextureLoad(const char* path);
	void loadAnimatedModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	AnimatedMesh processMesh(aiMesh *mesh, const aiScene *scene, unsigned int meshIndex);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	void AnimNodeProcess();
	Bone* FindBone(std::string in_name);
	aiNode* FindAiNode(std::string name);
	aiNodeAnim* FindAiNodeAnim(std::string name);
	int FindBoneIDByName(std::string name);
	void updateSkeleton(float deltaTime) {
		skeleton.updateBehaviour(deltaTime);
	};
	void AddAnimation(Animation& in_anim);
	Animation* FindAnimation(std::string anim_to_find);
	void PlayAnimation(Animation& anim, bool loop = false, bool reset_to_start = false);
	void StopAnimating();


private:
	std::string Path;
};