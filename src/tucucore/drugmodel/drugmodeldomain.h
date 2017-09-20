/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef DRUGMODELDOMAIN_H
#define DRUGMODELDOMAIN_H


#include "tucucore/operation.h"

namespace Tucuxi {
namespace Core {

class DrugModelDomain
{
public:

    DrugModelDomain(std::unique_ptr<Operation> _constraint);
    virtual ~DrugModelDomain();

    const Operation &getConstraint();

protected:
    std::unique_ptr<Operation> m_constraint;
};


}
}


#endif // DRUGMODELDOMAIN_H
