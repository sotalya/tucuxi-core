/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGMODEL_H
#define TUCUXI_CORE_DRUGMODEL_H

#include "tucucore/covariate.h"

namespace Tucuxi {
namespace Core {

class DrugModel
{
public:
    DrugModel();    

    const Covariates& getCovariates() const;

private:
    Covariates m_covariates;
};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H