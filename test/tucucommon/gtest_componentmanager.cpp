//@@license@@

#include <gtest/gtest.h>

#include "tucucommon/componentmanager.h"

#define PRINT_MESSAGE(x)    (std::cout << "\033[1;36m" << x << "\033[0m" << std::endl)


class ITest1 : public Tucuxi::Common::Interface
{
public:
    virtual void Test1() = 0;
};


class ITest2 : public Tucuxi::Common::Interface
{
public:
    virtual void Test2() = 0;
};


class TestComponent : public Tucuxi::Common::Component, ITest1, ITest2
{
public:
    static Tucuxi::Common::Interface* createComponent()
    {
        TestComponent* cmp = new TestComponent();
        return dynamic_cast<ITest1*>(cmp);
    }
    virtual void Test1()
    {
        PRINT_MESSAGE("Hello from Test1");
    }
    virtual void Test2()
    {
        PRINT_MESSAGE("Hello from Test2");
    }

protected:
    virtual Tucuxi::Common::Interface* getInterface(const std::string& _name)
    {
        return Tucuxi::Common::Component::getInterfaceImpl(_name);
    }

private:
    TestComponent()
    {
        registerInterface(dynamic_cast<ITest1*>(this));
        registerInterface(dynamic_cast<ITest2*>(this));
    }
};

TEST (Common_ComponentManagerTests, CreateComponent){

    Tucuxi::Common::ComponentManager* cmpMgr = Tucuxi::Common::ComponentManager::getInstance();
    cmpMgr->registerComponentFactory("TestComponent", &TestComponent::createComponent);
    ITest1* i1 = cmpMgr->createComponent<ITest1>("TestComponent");
    cmpMgr->registerComponent("Toto", i1);
    ITest2* i2 = cmpMgr->getComponent<ITest2>("Toto");

    EXPECT_TRUE(i1 != nullptr);
    EXPECT_TRUE(i2 != nullptr);

    EXPECT_NO_THROW(i1->Test1());
    EXPECT_NO_THROW(i2->Test2());

    ITest1* i3 = i2->getInterface<ITest1>();
    EXPECT_EQ(i1, i3);

    //i3 = nullptr;
    EXPECT_TRUE(i3 != nullptr);
    EXPECT_NO_THROW(i3->Test1());
}
