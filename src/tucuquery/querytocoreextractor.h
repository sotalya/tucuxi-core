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


#ifndef QUERYTOCOREEXTRACTOR_H
#define QUERYTOCOREEXTRACTOR_H


#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/covariateevent.h"
#include "tucucore/drugtreatment/sample.h"
#include "tucucore/targetevent.h"

#include "tucuquery/fullsample.h"
#include "tucuquery/querydata.h"

#include "querystatus.h"

namespace Tucuxi {

namespace Core {
class DrugTreatment;
class ComputingTrait;
class ComputingTraits;
class DrugModel;
} // namespace Core

namespace Query {

class ComputingQuery;

///
/// \brief The QueryToCoreExtractor class
/// This class is responsible for extracting data from a query and translate
/// various fields to Tucuxi::Core objects.
/// For instance, patient variates, targets and samples
///
class QueryToCoreExtractor
{
public:
    QueryToCoreExtractor();

    ///
    /// \brief Extract a ComputingQuery from the XML pre-processed data
    /// \param _query The QueryData object built from XML
    /// \param _computingQuery The extracted computing query
    /// \param _drugTreatments The extracted drug treatments
    /// \return A queryStatus
    ///
    /// This method should be the only public one.
    /// The drug treatments should live as long as the _computingQuery does.
    ///
    QueryStatus extractComputingQuery(
            const QueryData& _query,
            ComputingQuery& _computingQuery,
            std::vector<std::unique_ptr<Core::DrugTreatment> >& _drugTreatments);

    ///
    /// \brief extractPatientVariates
    /// \param _query
    /// \return
    ///
    Tucuxi::Core::PatientVariates extractPatientVariates(const QueryData& _query) const;

    ///
    /// \brief extractTargets
    /// \param _query
    /// \param _drugPosition
    /// \return
    ///
    Tucuxi::Core::Targets extractTargets(const QueryData& _query, size_t _drugPosition) const;

    ///
    /// \brief extractSamples
    /// \param _query
    /// \param _drugPosition
    /// \return
    ///
    Tucuxi::Query::FullSamples extractSamples(const QueryData& _query, size_t _drugPosition) const;

    std::unique_ptr<Core::DrugTreatment> extractDrugTreatment(
            const QueryData& _query, const RequestData& _requestData) const;

    Tucuxi::Core::DrugModel* extractDrugModel(
            const RequestData& _requestData, const Tucuxi::Core::DrugTreatment* _drugTreatment) const;

    std::string getErrorMessage() const
    {
        return m_errorMessage;
    }

    void setErrorMessage(std::string _errorMessage)
    {
        m_errorMessage = _errorMessage;
    }

private:
    std::string m_errorMessage;
};

} // namespace Query
} // namespace Tucuxi

#endif // QUERYTOCOREEXTRACTOR_H
