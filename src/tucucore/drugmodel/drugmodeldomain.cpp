/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "drugmodeldomain.h"
#include "tucucore/operation.h"


namespace Tucuxi {
namespace Core {


DrugModelDomain::DrugModelDomain(std::unique_ptr<Operation> _constraint) :
    m_constraint(std::move(_constraint)) {}

DrugModelDomain::~DrugModelDomain()
{

}

const Operation &DrugModelDomain::getConstraint() { return *m_constraint;}


}
}
