/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <gtest/gtest.h>

#include "tucucommon/component.h"
#include "tucucommon/componentmanager.h"

template<typename T>
constexpr void PRINT_MESSAGE(T _mess)
{
    std::cout << "\033[1;36m" << _mess << "\033[0m" << std::endl;
}
// #define PRINT_MESSAGE(x) (std::cout << "\033[1;36m" << x << "\033[0m" << std::endl)


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
    void Test1() override
    {
        PRINT_MESSAGE("Hello from Test1");
    }
    void Test2() override
    {
        PRINT_MESSAGE("Hello from Test2");
    }

protected:
    Tucuxi::Common::Interface* getInterface(const std::string& _name) override
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

TEST(Common_TestComponentManager, CreateComponent)
{

    Tucuxi::Common::ComponentManager* cmpMgr = Tucuxi::Common::ComponentManager::getInstance();
    cmpMgr->registerComponentFactory("TestComponent", &TestComponent::createComponent);
    ITest1* i1 = cmpMgr->createComponent<ITest1>("TestComponent");
    cmpMgr->registerComponent("Toto", i1);
    ITest2* i2 = cmpMgr->getComponent<ITest2>("Toto");

    ASSERT_TRUE(i1 != nullptr);
    ASSERT_TRUE(i2 != nullptr);

    ASSERT_NO_THROW(i1->Test1());
    ASSERT_NO_THROW(i2->Test2());

    ITest1* i3 = i2->getInterface<ITest1>();
    ASSERT_EQ(i1, i3);

    //i3 = nullptr;
    ASSERT_TRUE(i3 != nullptr);
    ASSERT_NO_THROW(i3->Test1());
}
