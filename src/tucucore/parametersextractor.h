/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_PARAMETERSEXTRACTOR_H
#define TUCUXI_CORE_PARAMETERSEXTRACTOR_H

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "tucucore/parameter.h"
#include "tucucore/covariateevent.h"

#include "tucucommon/utils.h"

class TestParameterExtractor;

namespace Tucuxi {
namespace Core {

class ParametersExtractor
{
public:
    /// \brief Create a Parameters Extractor for the specified interval, covariate events set, and parameter
    ///        definitions.
    ///        The time instants at which the parameter set events will be extracted is also computed at this stage.
    /// \param _covariates A list of covariate events.
    /// \param _drugParameters A list of parameter definitions.
    /// \param _start Start time of the considered interval.
    /// \param _end End time of the considered interval.
    /// \pre _start <= _end
    /// \pre No duplicates in _covariates.
    /// \pre No duplicates in _drugParameters.
    /// \warning _drugParameters gets modified by the call to this function! This is mandatory, as we
    ParametersExtractor(const CovariateSeries &_covariates,
                        ParameterDefinitions &_drugParameters,
                        const DateTime &_start,
                        const DateTime &_end);

    /// \brief Extract parameter set events in a given interval from covariate events and parameter definitions.
    /// \param _series Extracted set of parameter set events.
    /// \return EXIT_SUCCESS if the extraction was successful, EXIT_FAILURE otherwise.
    /// \warning No checks are made on the initial content of _series. If it contains already some events, they will
    ///          still be there at the end of the function. It is up to the user to guarantee the consistency of the
    ///          _series element.
    int extract(ParameterSetSeries &_series);

    // Make the test class friend, as this will allow us to test the helper methods (which are private).
    friend TestParameterExtractor;


private:
    /// \brief Set of parameter definitions, used to construct parameters from the covariate values.
    ParameterDefinitions &m_drugParameters;

    /// \brief Start date of the parameter extraction interval.
    const DateTime m_start;

    /// \brief End date of the parameter extraction interval.
    const DateTime m_end;

    /// \brief Collection of timed covariate events, used to build the set of parameters.
    std::map<DateTime, std::vector<std::pair<std::string, Value>>> m_timedCValues;

    /// \brief Operable Graph Manager, in charge of performing all the computations needed to derive parameter values.
    OperableGraphManager m_ogm;
};

}
}

#endif // TUCUXI_CORE_PARAMETERSEXTRACTOR_H
