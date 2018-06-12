/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "drugmodeldomain.h"
#include "tucucore/operation.h"


namespace Tucuxi {
namespace Core {


DrugModelDomain::DrugModelDomain(std::unique_ptr<Operation> _constraint)
{
    if (_constraint != nullptr)
        m_constraints.push_back(std::move(_constraint));
}

DrugModelDomain::~DrugModelDomain()
{

}

}
}
