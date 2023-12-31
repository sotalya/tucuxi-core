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


#include "tucucommon/componentmanager.h"

#include "fructose/fructose.h"

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
        printf("Test1\n");
    }
    virtual void Test2()
    {
        printf("Test2\n");
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


struct TestComponentManager : public fructose::test_base<TestComponentManager>
{
    void createComponent(const std::string& /*_testName*/)
    {
        Tucuxi::Common::ComponentManager* cmpMgr = Tucuxi::Common::ComponentManager::getInstance();
        cmpMgr->registerComponentFactory("TestComponent", &TestComponent::createComponent);
        ITest1* i1 = cmpMgr->createComponent<ITest1>("TestComponent");
        cmpMgr->registerComponent("Toto", i1);
        ITest2* i2 = cmpMgr->getComponent<ITest2>("Toto");

        fructose_assert(i1 != nullptr);
        fructose_assert(i2 != nullptr);

        fructose_assert_no_exception(i1->Test1());
        fructose_assert_no_exception(i2->Test2());

        ITest1* i3 = i2->getInterface<ITest1>();
        fructose_assert_eq(i1, i3);

        //i3 = nullptr;
        fructose_assert(i3 != nullptr);
        fructose_assert_no_exception(i3->Test1());
    }
};
