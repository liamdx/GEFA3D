#include "Object.h"

Object::Object(std::string name)
	:name(name)
{
	transform.identity();

}
void Object::updateBehaviour()
{
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->updateBehaviour();
	}
}

