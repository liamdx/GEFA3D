#include "Object.h"

Object::Object(std::string name)
	:name(name)
{
	//transform.identity();

}
void Object::updateBehaviour()
{
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->updateBehaviour();
	}
}

void GameObject::renderBehaviour()
{
	shader.use();
	
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->componentName == "Mesh")
		{
			//B * obj = new B();
			//auto obj2 = dynamic_cast<A*>(obj);
			auto meshComponent = dynamic_cast<EngineComponent*>(components[i].get());
			if (meshComponent != nullptr) {
				//Mesh& meshToRender = &meshComponent;
				//meshComponent.Draw(shader);
				std::cout << "B is an A" << std::endl;
			}
		}
	}
}