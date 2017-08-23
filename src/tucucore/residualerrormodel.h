/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_ROOT_RESIDUALERRORMODEL_H
#define TUCUXI_ROOT_RESIDUALERRORMODEL_H

#include "tucucore/definitions.h"
#include "tucucore/sample.h"
#include "tucucommon/general.h"

namespace Tucuxi {
namespace Core {

class IResidualErrorModel
{
public:

    virtual bool isEmpty() const = 0;
    virtual void applyEpsToArray(Concentrations &_concentrations, const Deviation &_eps) const = 0;

    virtual Value calculateSampleLikelihood(const Value _expected, const Value& _observed) const = 0;

};

class SigmaResidualErrorModel : public IResidualErrorModel
{
public:

    Sigma sigma;
/*    ResidualErrorType errorModel;
*/
    bool isEmpty() const { return true; };
    void applyEpsToArray(Concentrations &_concentrations, const Deviation &_eps) const {
        TMP_UNUSED_PARAMETER(_concentrations);
        TMP_UNUSED_PARAMETER(_eps);
    };

    Value calculateSampleLikelihood(const Value _expected, const Value& _observed) const {
        TMP_UNUSED_PARAMETER(_expected);
        TMP_UNUSED_PARAMETER(_observed);
        return 0.0;
    };

};

class EmptyResidualErrorModel : public IResidualErrorModel
{
public:
    bool isEmpty() const { return true;}

    void applyEpsToArray(Concentrations &_concentrations, const Deviation &_eps) const {
        TMP_UNUSED_PARAMETER(_concentrations);
        TMP_UNUSED_PARAMETER(_eps);
    };

    Value calculateSampleLikelihood(const Value _expected, const Value& _observed) const {
        TMP_UNUSED_PARAMETER(_expected);
        TMP_UNUSED_PARAMETER(_observed);
        return 0.0;
    };

};

//#define EMPTY_RESIDUAL_ERROR_MODEL ResidualErrorModel{EMPTY_SIGMA, ResidualErrorType::NONE}
// static const ResidualErrorModel EMPTY_RESIDUAL_ERROR_MODEL;
#define EMPTY_RESIDUAL_ERROR_MODEL EmptyResidualErrorModel()
} 
} 

#endif // TUCUXI_ROOT_RESIDUALERRORMODEL_H
