#pragma once
#include "Common.h"
class EngineComponent {
public:
	EngineComponent(const char* name) { componentName = name; };
	virtual ~EngineComponent() {};
	virtual void initBehaviour() {};
	virtual void startBehaviour() {};
	virtual void earlyUpdateBehaviour() {} ;
	virtual void updateBehaviour() {} ;
	virtual void uiBehaviour() {};

	const char* componentName;
};