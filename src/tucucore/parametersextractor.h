/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_PARAMETERSEXTRACTOR_H
#define TUCUXI_CORE_PARAMETERSEXTRACTOR_H

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "tucucommon/iterator.h"

#include "tucucore/parameter.h"
#include "tucucore/covariateevent.h"

#include "tucucommon/utils.h"

struct TestParameterExtractor;

namespace Tucuxi {
namespace Core {

class ParameterDefinitionIterator;

class ParametersExtractor
{
public:

    enum class Result {
        Ok,
        ExtractionError
    };

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
                        Tucuxi::Common::Iterator<const ParameterDefinition*> &paramsIterator,
                        const DateTime &_start,
                        const DateTime &_end);

    /// \brief Extract parameter set events in a given interval from covariate events and parameter definitions.
    /// \param _series Extracted set of parameter set events.
    /// \return Result::Ok if the extraction was successful, Result::ExtractionError otherwise.
    /// \warning No checks are made on the initial content of _series. If it contains already some events, they will
    ///          still be there at the end of the function. It is up to the user to guarantee the consistency of the
    ///          _series element.
    Result extract(ParameterSetSeries &_series);

    // Make the test class friend, as this will allow us to test the helper methods (which are private).
    friend TestParameterExtractor;


private:
    /// \brief Set of parameter definitions, used to construct parameters from the covariate values.
    Tucuxi::Common::Iterator<const ParameterDefinition*> &m_paramsIterator;

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
