//@@license@@


#ifndef COMPUTINGCOMPONENTFACTORY_H
#define COMPUTINGCOMPONENTFACTORY_H

#include "tucucore/computingservice/icomputingservice.h"

class ComputingComponentFactory
{
public:
    enum class CreationMode
    {
        Meta = 0,
        Single,
        Multi
#ifdef TUCU_COMPILE_MULTI
        ,
        Comparator
#endif // TUCU_COMPILE_MULTI
    };

    static void setMode(CreationMode _mode);

    static std::unique_ptr<Tucuxi::Core::IComputingService> createComputingService();

private:
    static CreationMode sm_currentMode;
};

#endif // COMPUTINGCOMPONENTFACTORY_H
