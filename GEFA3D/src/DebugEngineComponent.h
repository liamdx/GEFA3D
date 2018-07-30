#pragma once

#include "EngineComponent.h"

class DebugEngineComponent : public EngineComponent
{
public:
	DebugEngineComponent() : EngineComponent("DebugComponent") {};
	~DebugEngineComponent() override;
	void debugUpdateBehaviour();
	void updateBehaviour() override;
};