#pragma once
//namespace example {
class Example {
public:
	Example() {};

	virtual ~Example() {}
	virtual void init() {}
	virtual void start() {}
	virtual void earlyUpdate(float deltaTime) {}
	virtual void update(float deltaTime) {}
	virtual void UI() {};
};
//}