#pragma once

#include "GameObject.h"
#include "lighting\DirectionalLight.h"
#include "lighting\PointLight.h"
#include "lighting\SpotLight.h"
class Scene
{

public:
	Scene();
	Scene(std::vector<GameObject> gameObjects, DirectionalLight dirLight);
	Scene(std::vector<GameObject> gameObjects, DirectionalLight dirLight, std::vector<PointLight> pointLights);
	Scene(std::vector<GameObject> gameObjects, DirectionalLight dirLight, std::vector<PointLight> pointLights, std::vector<SpotLight> spotLights);

	void Draw(Shader shader);
	void AddGameObject(GameObject newObject);
	void RemoveGameObject(unsigned int objectId);
	void AddPointLight(PointLight newPointLight);
	void RemovePointLight(unsigned int pointLightId);
	void AddSpotLight(SpotLight newSpotLight);
	void RemoveSpotLight(unsigned int spotLightId);

private:
	std::vector<Shader> shaders;
	std::vector<GameObject> gameObjects;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	DirectionalLight dirLight;
};