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

    DrugModelDomain(std::unique_ptr<Operation> _constraint = nullptr);
    virtual ~DrugModelDomain();

    void addConstraint(std::unique_ptr<Operation> _constraint) { m_constraints.push_back(std::move(_constraint));}
    const std::vector<std::unique_ptr< Operation > >& getConstraints() const { return m_constraints;}

protected:
    std::vector<std::unique_ptr<Operation> > m_constraints;
};


}
}


#endif // DRUGMODELDOMAIN_H
