/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_CORE_PARAMETERSEXTRACTOR_H
#define TUCUXI_CORE_PARAMETERSEXTRACTOR_H

#include <map>
#include <vector>

#include "tucucommon/iterator.h"

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/covariateevent.h"
#include "tucucore/parameter.h"

class TestParameterExtractor;

namespace Tucuxi {
namespace Core {

class ParameterDefinitionIterator;

class ParametersExtractor
{
public:
    /// \brief Create a Parameters Extractor for the specified interval, covariate events set, and parameter
    ///        definitions.
    ///        The time instants at which the parameter set events will be extracted is also computed at this stage.
    /// \param _covariates A list of covariate events.
    /// \param _paramsIterator An iterator on the list of parameter definitions.
    /// \param _start Start time of the considered interval.
    /// \param _end End time of the considered interval.
    /// \pre _start <= _end
    /// \pre No duplicates in _covariates.
    /// \pre No duplicates in _drugParameters.
    /// \warning _drugParameters gets modified by the call to this function! This is mandatory, as we
    ParametersExtractor(
            const CovariateSeries& _covariates,
            Tucuxi::Common::Iterator<const ParameterDefinition*>& _paramsIterator,
            DateTime _start,
            DateTime _end);

    /// \brief Extract parameter set events in a given interval from covariate events and parameter definitions.
    /// The events will contain only the modified parameters. Therefore, in order to get events where every parameter
    /// is defined, you should call buildFullSet() after extract.
    /// \param _series Extracted set of parameter set events.
    /// \return Result::Ok if the extraction was successful, Result::ExtractionError otherwise.
    /// \warning No checks are made on the initial content of _series. If it contains already some events, they will
    ///          still be there at the end of the function. It is up to the user to guarantee the consistency of the
    ///          _series element.
    ComputingStatus extract(ParameterSetSeries& _series);

    ///
    /// \brief Builds a set with every parameter for each event
    /// \param _inputSeries The input series, only containing changes of parameters
    /// \param _outputSeries The output series that will contain every parameter for each event
    /// \return Result::Ok if the building was successful, Result::ExtractionError otherwise.
    /// \warning No cheks are made on the initial content of _inputSeries. This function is made to be called
    ///          after extract().
    ///
    ComputingStatus buildFullSet(const ParameterSetSeries& _inputSeries, ParameterSetSeries& _outputSeries) const;

    /// \brief Extract parameter set events in a given interval from population values of parameter definitions.
    /// \param _series Extracted set of parameter set events.
    /// \return Result::Ok if the extraction was successful, Result::ExtractionError otherwise.
    /// \warning No checks are made on the initial content of _series. If it contains already some events, they will
    ///          still be there at the end of the function. It is up to the user to guarantee the consistency of the
    ///          _series element.
    ComputingStatus extractPopulation(ParameterSetSeries& _series);

    // Make the test class friend, as this will allow us to test the helper methods (which are private).
    friend TestParameterExtractor;


private:
    /// \brief Set of parameter definitions, used to construct parameters from the covariate values.
    Tucuxi::Common::Iterator<const ParameterDefinition*>& m_paramsIterator;

    /// \brief Start date of the parameter extraction interval.
    const DateTime m_start;

    /// \brief End date of the parameter extraction interval.
    const DateTime m_end;

    /// \brief Collection of timed covariate events, used to build the set of parameters.
    std::map<DateTime, std::vector<std::pair<std::string, Value>>> m_timedCValues;

    /// \brief Operable Graph Manager, in charge of performing all the computations needed to derive parameter values.
    OperableGraphManager m_ogm;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_PARAMETERSEXTRACTOR_H
