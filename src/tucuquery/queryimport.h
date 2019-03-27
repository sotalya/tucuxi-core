#ifndef TUCUXI_QUERY_QUERYIMPORT_H
#define TUCUXI_QUERY_QUERYIMPORT_H

#include <string>
#include <vector>
#include <mutex>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/iimport.h"
#include "query.h"

namespace Tucuxi {
namespace Query {

class QueryImport : public Tucuxi::Common::IImport
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
    Result importFromFile(Tucuxi::Query::Query *&_query, std::string _fileName);

    ///
    /// \brief importFromString
    /// \param _query A reference to a query pointer that will be allocated within the function
    /// \param _xml A string in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Result importFromString(Tucuxi::Query::Query *&_query, std::string _xml);

protected:

    const std::vector<std::string> &ignoredTags() const override;

    /// A mutex to ensure the public methods are reentrant
    std::mutex m_mutex;

    ///
    /// \brief importFromString
    /// \param _query A reference to a query pointer that will be allocated within the function
    /// \param _document An XmlDocument in which the query is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Result importDocument(Tucuxi::Query::Query *&_query, Tucuxi::Common::XmlDocument & _document);

protected:
    // Methods to separate the creation of an AdministrativeData
    std::unique_ptr<AdministrativeData> createAdministrativeData(Common::XmlDocument &_document) const;
    std::unique_ptr<Person> createPerson(Common::XmlNodeIterator& _personRootIterator) const;
    std::unique_ptr<PersonalContact> createPersonalContact(Common::XmlNodeIterator& _personalContactRootIterator) const;
    std::unique_ptr<InstituteContact> createInstituteContact(Common::XmlNodeIterator& _instituteContactRootIterator) const;
    std::unique_ptr<Address> createAddress(Common::XmlNodeIterator& _addressRootIterator) const;
    std::unique_ptr<Phone> createPhone(Common::XmlNodeIterator& _phoneRootIterator) const;
    std::unique_ptr<Email> createEmail(Common::XmlNodeIterator& _emailRootIterator) const;
    std::unique_ptr<ClinicalData> createClinicalData(Common::XmlNodeIterator& _clinicalDataRootIterator) const;

    // Methods to separate the creation of a ParametersData
    std::unique_ptr<ParametersData> createParametersData(Tucuxi::Common::XmlDocument & _document) const;
    std::unique_ptr<PatientData> createPatientData(Common::XmlNodeIterator& _patientDataRootIterator) const;
    std::unique_ptr<CovariateData> createCovariateData(Common::XmlNodeIterator& _covariateDataRootIterator) const;
    std::unique_ptr<DrugData> createDrugData(Common::XmlNodeIterator& _drugDataRootIterator) const;
    std::unique_ptr<TargetData> createTargetData(Common::XmlNodeIterator& _targetDataRootIterator) const;
    std::unique_ptr<SampleData> createSampleData(Common::XmlNodeIterator& _sampleDataRootIterator) const;
    std::unique_ptr<ConcentrationData> createConcentrationData(Common::XmlNodeIterator& _concentrationDataRootIterator) const;
    std::unique_ptr<Treatment> createTreatment(Common::XmlNodeIterator& _treatmentRootIterator) const;
    std::unique_ptr<Core::DosageTimeRange> createDosageTimeRange(Common::XmlNodeIterator& _dosageTimeRangeRootIterator) const;
    std::unique_ptr<Core::Dosage> createDosage(Common::XmlNodeIterator& _dosageRootIterator) const;
    std::unique_ptr<Core::DosageBounded> createDosageBounded(Common::XmlNodeIterator& _dosageBoundedRootIterator) const;
    std::unique_ptr<Core::FormulationAndRoute> createFormulationAndRoute(Common::XmlNodeIterator& _formulationAndRouteRootIterator) const;

    // Methods to separate the creation of the requests
    std::unique_ptr<RequestData> createRequest(Common::XmlNodeIterator& _requestRootIterator) const;
    std::unique_ptr<GraphData> createGraphData(Common::XmlNodeIterator& _graphDataRootIterator) const;
    std::unique_ptr<Backextrapolation> createBackextrapolation(Common::XmlNodeIterator& _backextrapolationRootIterator) const;

    // Utilitary methods
    const std::string getChildStringValue(Common::XmlNodeIterator _rootIterator, std::string _childName) const;
    int getChildIntValue(Common::XmlNodeIterator _rootIterator, std::string _childName) const;
    double getChildDoubleValue(Common::XmlNodeIterator _rootIterator, std::string _childName) const;
    bool getChildBoolValue(Common::XmlNodeIterator _rootIterator, std::string _childName) const;
    Common::DateTime getChildDateTimeValue(Common::XmlNodeIterator _rootIterator, std::string _childName) const;
    Common::Duration getChildDuration(Common::XmlNodeIterator _rootIterator, std::string _childName) const;

};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_QUERYIMPORT_H
