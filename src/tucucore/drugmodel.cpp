
/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/drugmodel.h"
#include "tucucore/covariate.h"

namespace Tucuxi {
namespace Core {

DrugModel::DrugModel()
{
}


const CovariateDefinitions& DrugModel::getCovariates() const
{
    return m_covariates;
}

}
}
