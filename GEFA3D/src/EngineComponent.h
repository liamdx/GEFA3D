#pragma once
#include "Common.h"
class EngineComponent {
public:
	EngineComponent(const char* name) { componentName = name; };
	virtual ~EngineComponent() {};
	virtual void updateBehaviour() {} ;
	virtual void renderBehaviour() {} ;
	const char* componentName;
};