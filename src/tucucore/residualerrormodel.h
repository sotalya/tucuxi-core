/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_ROOT_RESIDUALERRORMODEL_H
#define TUCUXI_ROOT_RESIDUALERRORMODEL_H

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

class ResidualErrorModel
{
public:
/*
    sigma_t sigma;
    ResidualErrorType errorModel;
*/
    bool isEmpty() const { return true; };
    void applyEpsToArray(Concentrations &_concentrations, const Deviation &eps) const {};
/*
    loglikelihood_t calculateSampleLikelihood(const cxn_t _y, const Sample& samp) const;
*/
};

//#define EMPTY_RESIDUAL_ERROR_MODEL ResidualErrorModel{EMPTY_SIGMA, ResidualErrorType::NONE}
static const ResidualErrorModel EMPTY_RESIDUAL_ERROR_MODEL;

} 
} 

#endif // TUCUXI_ROOT_RESIDUALERRORMODEL_H
