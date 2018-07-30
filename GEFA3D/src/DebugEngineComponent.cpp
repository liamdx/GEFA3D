#include "DebugEngineComponent.h"


void DebugEngineComponent::debugUpdateBehaviour()
{
	//std::cout << EngineComponent::componentName << std::endl;
	std::cout << "IF ^ says debug, base class working" << std::endl;
}

void DebugEngineComponent::updateBehaviour()
{
	std::cout << "Inherited Class Working" << std::endl;
}

DebugEngineComponent::~DebugEngineComponent()
{

}