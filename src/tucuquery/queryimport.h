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


#ifndef TUCUXI_QUERY_QUERYIMPORT_H
#define TUCUXI_QUERY_QUERYIMPORT_H

#include <mutex>
#include <string>
#include <vector>

#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/xmlnode.h"

#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/definitions.h"
#include "tucucore/drugmodel/analyte.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/drugtreatment/sample.h"
#include "tucucore/drugtreatment/target.h"

#include "tucuquery/fullsample.h"

#include "querydata.h"

class GTestTucuQuery;

namespace Tucuxi {
namespace Query {


class QueryImport : public Tucuxi::Common::XMLImporter
{
public:
    ///
    /// \brief QueryImport empty constructor
    ///
    QueryImport();

    virtual ~QueryImport();

    ///
    /// \brief importFromFile
    /// \param _query A reference to a query pointer that will be allocated within the function
    /// \param _fileName The name of the file in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importFromFile(std::unique_ptr<Tucuxi::Query::QueryData>& _query, const std::string& _fileName);

    ///
    /// \brief importFromString
    /// \param _query A reference to a query pointer that will be allocated within the function
    /// \param _xml A string in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importFromString(std::unique_ptr<Tucuxi::Query::QueryData>& _query, const std::string& _xml);

protected:
    const std::vector<std::string>& ignoredTags() const override;

    /// A mutex to ensure the public methods are reentrant
    std::mutex m_mutex;

    ///
    /// \brief importFromString
    /// \param _query A reference to a query pointer that will be allocated within the function
    /// \param _document An XmlDocument in which the query is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importDocument(std::unique_ptr<Tucuxi::Query::QueryData>& _query, Tucuxi::Common::XmlDocument& _document);

    // Methods to separate the creation of a ParametersData
    std::unique_ptr<DrugTreatmentData> createDrugTreatmentData(Tucuxi::Common::XmlDocument& _document);
    std::unique_ptr<PatientData> createPatientData(Common::XmlNodeIterator& _patientDataRootIterator);
    std::unique_ptr<Core::PatientCovariate> createCovariateData(Common::XmlNodeIterator& _covariateDataRootIterator);
    std::unique_ptr<DrugData> createDrugData(Common::XmlNodeIterator& _drugDataRootIterator);
    std::unique_ptr<Tucuxi::Core::Target> createTargetData(Common::XmlNodeIterator& _targetDataRootIterator);
    std::unique_ptr<Tucuxi::Query::FullSample> createSampleData(
            Common::XmlNodeIterator& _sampleDataRootIterator, Common::XmlNodeIterator& _concentrationRootIterator);
    //    std::unique_ptr<Tucuxi::Core::ConcentrationData> createConcentrationData(Common::XmlNodeIterator& _concentrationDataRootIterator) ;
    std::unique_ptr<Treatment> createTreatment(Common::XmlNodeIterator& _treatmentRootIterator);
    std::unique_ptr<Core::DosageTimeRange> createDosageTimeRange(Common::XmlNodeIterator& _dosageTimeRangeRootIterator);
    std::unique_ptr<Core::Dosage> createDosage(Common::XmlNodeIterator& _dosageRootIterator);
    std::unique_ptr<Core::DosageBounded> createDosageBounded(Common::XmlNodeIterator& _dosageBoundedRootIterator);
    std::unique_ptr<Core::DosageBounded> createDosageBoundedFromIterator(
            Common::XmlNodeIterator& _dosageBoundedIterator);
    std::unique_ptr<Core::FormulationAndRoute> createFormulationAndRoute(
            Common::XmlNodeIterator& _formulationAndRouteRootIterator);

    // Methods to separate the creation of the requests
    std::unique_ptr<RequestData> createRequest(Common::XmlNodeIterator& _requestRootIterator);

    // Utilitary methods
    Tucuxi::Core::PercentileRanks getChildPercentileRanks(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    std::vector<Common::DateTime> getChildDateTimeList(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName) const;
    std::unique_ptr<Tucuxi::Core::ComputingTraitAdjustment> getChildComputingTraitAdjustment(
            Common::XmlNodeIterator _rootIterator, const std::string&);
    std::unique_ptr<Tucuxi::Core::ComputingTraitConcentration> getChildComputingTraitConcentration(
            Common::XmlNodeIterator _rootIterator, const std::string&);
    std::unique_ptr<Tucuxi::Core::ComputingTraitPercentiles> getChildComputingTraitPercentiles(
            Common::XmlNodeIterator _rootIterator, const std::string&);
    std::unique_ptr<Tucuxi::Core::ComputingTraitSinglePoints> getChildComputingTraitSinglePoints(
            Common::XmlNodeIterator _rootIterator, const std::string&);
    std::unique_ptr<Tucuxi::Core::ComputingTraitAtMeasures> getChildComputingTraitAtMeasures(
            Common::XmlNodeIterator _rootIterator, const std::string&);
    Tucuxi::Core::ComputingOption getChildComputingOption(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    Tucuxi::Core::PredictionParameterType getChildParametersTypeEnum(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    Tucuxi::Core::CompartmentsOption getChildCompartmentsOptionEnum(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    Tucuxi::Core::BestCandidatesOption getChildBestCandidatesOptionEnum(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    Tucuxi::Core::LoadingOption getChildLoadingOptionEnum(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    Tucuxi::Core::RestPeriodOption getChildRestPeriodTargetOptionEnum(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    Tucuxi::Core::SteadyStateTargetOption getChildSteadyStateTargetOptionEnum(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    Tucuxi::Core::TargetExtractionOption getChildTargetExtractionOptionEnum(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);
    Tucuxi::Core::FormulationAndRouteSelectionOption getChildFormulationAndRouteSelectionOptionEnum(
            Common::XmlNodeIterator _rootIterator, const std::string& _childName);

private:
    friend GTestTucuQuery;
};


} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_QUERYIMPORT_H
