/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_PARAMETERSEXTRACTOR_H
#define TUCUXI_CORE_PARAMETERSEXTRACTOR_H

#include "tucucore/parameter.h"
#include "tucucore/covariate.h"

namespace Tucuxi {
namespace Core {

class ParametersExtractor
{
public:

    ///
    /// \brief extract Extract parameters, by calculating them based on covariates
    /// \param _covariates A list of covariates
    /// \param _drugParameters A list of drug parameters
    /// \param _start From this date
    /// \param _end To this date
    /// \param _series Output parameters
    /// \return An error code
    /// This method shall extract the parameters each time covariates change. It has to use
    /// the operations in order to do so.
    static int extract(const CovariateSeries &_covariates, const ParameterDefinitions& _drugParameters, const DateTime &_start, const DateTime &_end, ParameterSetSeries &_series);
};

}
}

#endif // TUCUXI_CORE_PARAMETERSEXTRACTOR_H
