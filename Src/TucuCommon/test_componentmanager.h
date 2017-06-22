#ifndef TUCUXI_TUCUCOMMON_TEST_COMPONENTMANAGER_H
#define TUCUXI_TUCUCOMMON_TEST_COMPONENTMANAGER_H

#include "componentmanager.h"

namespace Tucuxi {
namespace Common {

class ITest1 : public Interface
{
public:
	virtual void Test1() = 0;
};

class ITest2 : public Interface
{
public:
	virtual void Test2() = 0;
};

class TestComponent : public Component, ITest1, ITest2
{
public:
	static Interface* createComponent()
	{
		TestComponent *cmp = new TestComponent();
		return dynamic_cast<ITest1*>(cmp);
	}
	virtual void Test1() { printf("Test1\n"); }
	virtual void Test2() { printf("Test2\n"); }

protected:
	virtual Interface* getInterface(const std::string &_name) { return Component::getInterface(_name); }

private:
	TestComponent() {
		registerInterface(dynamic_cast<ITest1*>(this));
		registerInterface(dynamic_cast<ITest2*>(this));
	}
};

void TestComponentManager()
{
	ComponentManager* cmpMgr = ComponentManager::getInstance();
	cmpMgr->registerComponentFactory("TestComponent", &TestComponent::createComponent);
	ITest1* i1 = cmpMgr->createComponent<ITest1>("TestComponent");
	cmpMgr->registerComponent("Toto", i1);
	ITest2* i2 = cmpMgr->getComponent<ITest2>("Toto");

	i1->Test1();
	i2->Test2();

	i1 = i2->getInterface<ITest1>();
	i1->Test1();
}

}
}

#endif // TUCUXI_TUCUCOMMON_TEST_COMPONENTMANAGER_H