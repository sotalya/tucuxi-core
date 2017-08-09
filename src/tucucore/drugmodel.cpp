
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


const Covariates& DrugModel::getCovariates() const
{
    return m_covariates;
}

}
}