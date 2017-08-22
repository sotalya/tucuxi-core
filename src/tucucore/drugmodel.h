/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGMODEL_H
#define TUCUXI_CORE_DRUGMODEL_H

#include "tucucore/covariate.h"
#include "tucucore/parameter.h"

namespace Tucuxi {
namespace Core {

class DrugErrorModel
{
    // TODO YJE
};

class DrugModel
{
public:
    DrugModel();    

    const Covariates& getCovariates() const;
    const DrugErrorModel& getErrorModel() const { return m_errorModel; }
    const ParameterDefinitions& getParemeters() { return m_parameters; }

private:
    ParameterDefinitions m_parameters;
    Covariates m_covariates;
    DrugErrorModel m_errorModel;
};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H