#include "queryimport.h"

#include <boost/algorithm/string.hpp>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/utils.h"


#include <unistd.h>
#include <chrono>

#include "tucucore/definitions.h"

using namespace std;
using namespace Tucuxi;

namespace Tucuxi {
namespace Query {

const string QueryImport::m_sDATE_FORMAT = "%Y-%m-%dT%H:%M:%S";


QueryImport::QueryImport()
{

}


QueryImport::~QueryImport()
{

}

const std::vector<std::string> &QueryImport::ignoredTags() const
{
    static std::vector<std::string> ignored;
    return ignored;
}

QueryImport::Result QueryImport::importFromFile(Tucuxi::Query::Query *&_query, std::string _fileName)
{
    // Ensure the function is reentrant
    std::lock_guard<std::mutex> lock(m_mutex);

    setResult(Result::Ok);
    _query = nullptr;

    Tucuxi::Common::XmlDocument document;
    if (!document.open(_fileName)) {
        return Result::CantOpenFile;
    }

    return importDocument(_query, document);
}


QueryImport::Result QueryImport::importFromString(Tucuxi::Query::Query *&_query, std::string _xml)
{
    // Ensure the function is reentrant
    std::lock_guard<std::mutex> lock(m_mutex);

    setResult(Result::Ok);
    _query = nullptr;

    Tucuxi::Common::XmlDocument document;

    if (!document.fromString(_xml)) {
        return Result::Error;
    }

    return importDocument(_query, document);
}


///////////////////////////////////////////////////////////////////////////////
/// General methods
///////////////////////////////////////////////////////////////////////////////

QueryImport::Result QueryImport::importDocument(
        Tucuxi::Query::Query *&_query,
        Tucuxi::Common::XmlDocument & _document)
{

    // TODO try to see if I can catch a parsing exception.

    const string DRUG_ID_NODE_NAME = "queryID";
    const string CLIENT_ID_NODE_NAME = "clientID";
    const string DATE_NODE_NAME = "date";
    const string LANGUAGE_NODE_NAME = "language";
    const string REQUESTS_NODE_NAME = "requests";

    Common::XmlNode root = _document.getRoot();

    string drugID = root.getChildren(DRUG_ID_NODE_NAME)->getValue();
    string clientID = root.getChildren(CLIENT_ID_NODE_NAME)->getValue();

    string dateValue = root.getChildren(DATE_NODE_NAME)->getValue();
    Common::DateTime date(dateValue, m_sDATE_FORMAT);

    string language = root.getChildren(LANGUAGE_NODE_NAME)->getValue();

    unique_ptr<AdministrativeData> pAdministrativeData = createAdministrativeData(_document);
    unique_ptr<ParametersData> pParametersData = createParametersData(_document);

    Common::XmlNodeIterator requestsRootIterator = root.getChildren(REQUESTS_NODE_NAME);
    Common::XmlNodeIterator requestsIterator = requestsRootIterator->getChildren();
    vector< unique_ptr<RequestData> > requests;
    while(requestsIterator != requestsIterator.none()) {
        requests.emplace_back(createRequest(requestsIterator));
        requestsIterator++;
    }

    _query = new Query(
                drugID,
                clientID,
                date,
                language,
                move(pAdministrativeData),
                move(pParametersData),
                requests
                );


    return getResult();

}


unique_ptr<AdministrativeData> QueryImport::createAdministrativeData(Tucuxi::Common::XmlDocument & _document) const
{
    const string ADMIN_NODE_NAME = "admin";
    const string MANDATOR_NODE_NAME = "mandator";
    const string PATIENT_NODE_NAME = "patient";
    const string CLINICALDATA_NODE_NAME = "clinicalData";

    Common::XmlNode root = _document.getRoot();
    Common::XmlNodeIterator adminRootIterator = root.getChildren(ADMIN_NODE_NAME);

    if (!adminRootIterator->isValid()) {
        // TODO
        //return nullptr;
    }

    Common::XmlNodeIterator mandatorRootIterator = adminRootIterator->getChildren(MANDATOR_NODE_NAME);
    Common::XmlNodeIterator patientRootIterator  = adminRootIterator->getChildren(PATIENT_NODE_NAME);
    Common::XmlNodeIterator clinicalDataRootIterator  = adminRootIterator->getChildren(CLINICALDATA_NODE_NAME);

    unique_ptr<Person> pMandator = createPerson(mandatorRootIterator);
    unique_ptr<Person> pPatient = createPerson(patientRootIterator);
    unique_ptr<ClinicalData> pClinicalData = createClinicalData(clinicalDataRootIterator);

    return make_unique<AdministrativeData>(
                                      move(pMandator),
                                      move(pPatient),
                                      move(pClinicalData)
                                    );
}

unique_ptr<Person> QueryImport::createPerson(Common::XmlNodeIterator& _personRootIterator) const
{
    const string PERSON_NODE_NAME = "person";
    const string INSTITUTE_NODE_NAME = "institute";

    Common::XmlNodeIterator iterator = _personRootIterator->getChildren(PERSON_NODE_NAME);
    unique_ptr<PersonalContact> pPersonalContact = createPersonalContact(iterator);
    iterator = _personRootIterator->getChildren(INSTITUTE_NODE_NAME);
    unique_ptr<InstituteContact> pInstituteContact = createInstituteContact(iterator);

    return make_unique<Person>(
                           move(pPersonalContact),
                           move(pInstituteContact)
                       );
}

unique_ptr<PersonalContact> QueryImport::createPersonalContact(Common::XmlNodeIterator& _personalContactRootIterator) const
{
    const string ID_NODE_NAME = "id";
    const string TITLE_NODE_NAME = "title";
    const string FIRSTNAME_NODE_NAME = "firstName";
    const string LASTNAME_NODE_NAME = "lastName";
    const string ADDRESS_NODE_NAME = "address";
    const string PHONE_NODE_NAME = "phone";
    const string EMAIL_NODE_NAME = "email";

    string id = getChildStringValue(_personalContactRootIterator, ID_NODE_NAME);
    string title = getChildStringValue(_personalContactRootIterator, TITLE_NODE_NAME);
    string firstname = getChildStringValue(_personalContactRootIterator, FIRSTNAME_NODE_NAME);
    string lastname = getChildStringValue(_personalContactRootIterator, LASTNAME_NODE_NAME);
    Common::XmlNodeIterator iterator = _personalContactRootIterator->getChildren(ADDRESS_NODE_NAME);
    unique_ptr<Address> pAddress = createAddress(iterator);
    iterator = _personalContactRootIterator->getChildren(PHONE_NODE_NAME);
    unique_ptr<Phone> pPhone = createPhone(iterator);
    iterator = _personalContactRootIterator->getChildren(EMAIL_NODE_NAME);
    unique_ptr<Email> pEmail = createEmail(iterator);

    return make_unique<PersonalContact>(
                                      id,
                                      title,
                                      firstname,
                                      lastname,
                                      move(pAddress),
                                      move(pPhone),
                                      move(pEmail)
                                    );
}

unique_ptr<InstituteContact> QueryImport::createInstituteContact(Common::XmlNodeIterator& _instituteContactRootIterator) const
{
    const string ID_NODE_NAME = "id";
    const string NAME_NODE_NAME = "name";
    const string ADDRESS_NODE_NAME = "address";
    const string PHONE_NODE_NAME = "phone";
    const string EMAIL_NODE_NAME = "email";

    string id = getChildStringValue(_instituteContactRootIterator, ID_NODE_NAME);
    string name = getChildStringValue(_instituteContactRootIterator, NAME_NODE_NAME);
    Common::XmlNodeIterator iterator = _instituteContactRootIterator->getChildren(ADDRESS_NODE_NAME);
    unique_ptr<Address> pAddress = createAddress(iterator);
    iterator = _instituteContactRootIterator->getChildren(PHONE_NODE_NAME);
    unique_ptr<Phone> pPhone = createPhone(iterator);
    iterator = _instituteContactRootIterator->getChildren(EMAIL_NODE_NAME);
    unique_ptr<Email> pEmail = createEmail(iterator);

    return make_unique<InstituteContact>(
                                      id,
                                      name,
                                      move(pAddress),
                                      move(pPhone),
                                      move(pEmail)
                                      );
}

unique_ptr<Address> QueryImport::createAddress(Common::XmlNodeIterator& _addressRootIterator) const
{
    const string STREET_NODE_NAME = "street";
    const string POSTALCODE_NODE_NAME = "postCode";
    const string CITY_NODE_NAME = "city";
    const string STATE_NODE_NAME = "state";
    const string COUNTRY_NODE_NAME = "country";

    string street = getChildStringValue(_addressRootIterator, STREET_NODE_NAME);
    int postalCode = getChildIntValue(_addressRootIterator, POSTALCODE_NODE_NAME);
    string city = getChildStringValue(_addressRootIterator, CITY_NODE_NAME);
    string state = getChildStringValue(_addressRootIterator, STATE_NODE_NAME);
    string country = getChildStringValue(_addressRootIterator, COUNTRY_NODE_NAME);

    return make_unique<Address>(
                            street,
                            postalCode,
                            city,
                            state,
                            country
                         );
}

unique_ptr<Phone> QueryImport::createPhone(Common::XmlNodeIterator& _phoneRootIterator) const
{
    const string NUMBER_NODE_NAME = "address";
    const string TYPE_NODE_NAME = "type";

    string number = getChildStringValue(_phoneRootIterator, NUMBER_NODE_NAME);
    string type = getChildStringValue(_phoneRootIterator, TYPE_NODE_NAME);

    return make_unique<Phone>(number, type);
}

unique_ptr<Email> QueryImport::createEmail(Common::XmlNodeIterator& _emailRootIterator) const
{
    const string ADDRESS_NODE_NAME = "address";
    const string TYPE_NODE_NAME = "type";

    // Do the pre treatment TODO

    string address = getChildStringValue(_emailRootIterator, ADDRESS_NODE_NAME);
    string type = getChildStringValue(_emailRootIterator, TYPE_NODE_NAME);

    return make_unique<Email>(address, type);

    // Do the post treatment TODO
}

unique_ptr<ClinicalData> QueryImport::createClinicalData(Common::XmlNodeIterator& _clinicalDataRootIterator) const
{
    map<string, string> data;

    Common::XmlNodeIterator xmlNodeIterator = _clinicalDataRootIterator->getChildren();
    while(xmlNodeIterator != xmlNodeIterator.none()) {
        data[xmlNodeIterator->getName()] = xmlNodeIterator->getValue();
        xmlNodeIterator++;
    }

    return make_unique<ClinicalData>(data);
}

unique_ptr<ParametersData> QueryImport::createParametersData(Tucuxi::Common::XmlDocument & _document) const
{
    const string PARAMETERS_NODE_NAME = "parameters";
    const string PATIENT_NODE_NAME = "patient";
    const string DRUGS_NODE_NAME = "drugs";

    Common::XmlNode root = _document.getRoot();
    Common::XmlNodeIterator parametersRootIterator = root.getChildren(PARAMETERS_NODE_NAME);

    if (!parametersRootIterator->isValid()) {
        // TODO
        //return nullptr;
    }

    Common::XmlNodeIterator patientRootIterator = parametersRootIterator->getChildren(PATIENT_NODE_NAME);
    unique_ptr<PatientData> pPatient = createPatientData(patientRootIterator);

    vector< unique_ptr<DrugData> > drugs;
    Common::XmlNodeIterator drugsRootIterator  = parametersRootIterator->getChildren(DRUGS_NODE_NAME);
    Common::XmlNodeIterator drugsIterator = drugsRootIterator->getChildren();
    while(drugsIterator != drugsIterator.none()) {
        drugs.push_back(createDrugData(drugsIterator));
        drugsIterator++;
    }

    return make_unique<ParametersData>(
                                      move(pPatient),
                                      move(drugs)
                                    );
}

unique_ptr<PatientData> QueryImport::createPatientData(Common::XmlNodeIterator& _patientDataRootIterator) const
{
    const string COVARIATES_NODE_NAME = "covariates";
    const string COVARIATE_NODE_NAME = "covariate";

    vector< unique_ptr<CovariateData> > covariates;

    Common::XmlNodeIterator covariatesRootIterator = _patientDataRootIterator->getChildren(COVARIATES_NODE_NAME);
    Common::XmlNodeIterator covariatesIterator = covariatesRootIterator->getChildren();
    while(covariatesIterator != covariatesIterator.none()) {
        covariates.push_back(createCovariateData(covariatesIterator));
        covariatesIterator++;
    }

    return make_unique<PatientData>(covariates);
}

unique_ptr<CovariateData> QueryImport::createCovariateData(Common::XmlNodeIterator& _covariateDataRootIterator) const
{
    const string NAME_NODE_NAME = "name";
    const string DATE_NODE_NAME = "date";
    const string VALUE_NODE_NAME = "value";
    const string UNIT_NODE_NAME = "unit";
    const string DATATYPE_NODE_NAME = "dataType";
    const string NATURE_NODE_NAME = "nature";

    string name = getChildStringValue(_covariateDataRootIterator, NAME_NODE_NAME);
    Common::DateTime date = getChildDateTimeValue(_covariateDataRootIterator, DATE_NODE_NAME);
    string value = getChildStringValue(_covariateDataRootIterator, VALUE_NODE_NAME);
    string unit = getChildStringValue(_covariateDataRootIterator, UNIT_NODE_NAME);

    string dataTypeString = getChildStringValue(_covariateDataRootIterator, DATATYPE_NODE_NAME);
    Core::DataType dataType;
    if (dataTypeString == "int") {
        dataType = Core::DataType::Int;
    } else if (dataTypeString == "double") {
        dataType = Core::DataType::Double;
    } else if (dataTypeString == "bool") {
        dataType = Core::DataType::Bool;
    } else if (dataTypeString == "date") {
        dataType = Core::DataType::Date;
    } else {
        // TODO
    }

    // If the covariate is a date, go through a DateTime to reconvert it to a string
    // to be sure it has the correct format
    if (dataType == Core::DataType::Date) {
        Common::DateTime valueAsDate;
        valueAsDate = getChildDateTimeValue(_covariateDataRootIterator, VALUE_NODE_NAME);
        value = Tucuxi::Common::Utils::varToString(valueAsDate);
    }

    string nature = getChildStringValue(_covariateDataRootIterator, NATURE_NODE_NAME);

    return make_unique<CovariateData>(
                                    name,
                                     date,
                                     value,
                                     unit,
                                     dataType,
                                     nature
                                );
}

unique_ptr<DrugData> QueryImport::createDrugData(Common::XmlNodeIterator& _drugDataRootIterator) const
{
    const string DRUG_ID_NODE_NAME = "drugID";
    const string DRUG_MODEL_ID_NODE_NAME = "drugModelID";
    const string ACTIVE_PRINCIPLE_NODE_NAME = "activePrinciple";
    const string BRAND_NAME_NODE_NAME = "brandName";
    const string ATC_NODE_NAME = "atc";
    const string TREATMENT_NODE_NAME = "treatment";
    const string SAMPLES_NODE_NAME = "samples";
    const string TARGETS_NODE_NAME = "targets";

    string drugID = getChildStringValue(_drugDataRootIterator, DRUG_ID_NODE_NAME);
    string drugModelID = getChildStringValue(_drugDataRootIterator, DRUG_MODEL_ID_NODE_NAME);
    string activePrinciple = getChildStringValue(_drugDataRootIterator, ACTIVE_PRINCIPLE_NODE_NAME);
    string brandName = getChildStringValue(_drugDataRootIterator, BRAND_NAME_NODE_NAME);
    string atc = getChildStringValue(_drugDataRootIterator, ATC_NODE_NAME);

    Common::XmlNodeIterator treatmentRootIterator = _drugDataRootIterator->getChildren(TREATMENT_NODE_NAME);
    unique_ptr<Treatment> pTreatment = createTreatment(treatmentRootIterator);

    vector< unique_ptr<SampleData> > samples;
    Common::XmlNodeIterator samplesRootIterator = _drugDataRootIterator->getChildren(SAMPLES_NODE_NAME);
    Common::XmlNodeIterator samplesIterator = samplesRootIterator->getChildren();
    while(samplesIterator != samplesIterator.none()) {
        samples.push_back(createSampleData(samplesIterator));
        samplesIterator++;
    }

    vector< unique_ptr<TargetData> > targets;
    Common::XmlNodeIterator targetsRootIterator = _drugDataRootIterator->getChildren(TARGETS_NODE_NAME);
    Common::XmlNodeIterator targetsIterator = targetsRootIterator->getChildren();
    while(targetsIterator != targetsIterator.none()) {
        targets.push_back(createTargetData(targetsIterator));
        targetsIterator++;
    }

    return make_unique<DrugData>(
                            drugID,
                            drugModelID,
                            activePrinciple,
                            brandName,
                            atc,
                            move(pTreatment),
                            samples,
                            targets
                        );
}

unique_ptr<TargetData> QueryImport::createTargetData(Common::XmlNodeIterator& _targetDataRootIterator) const
{
    const string ANALYTE_ID_NODE_NAME           = "activeMoietyID";
    const string TARGET_TYPE_NODE_NAME          = "targetType";
    const string UNIT_NODE_NAME                 = "unit";
    const string INEFFICACY_ALARM_ID_NODE_NAME  = "inefficacyAlarm";
    const string MIN_NODE_NAME                  = "min";
    const string BEST_NODE_NAME                 = "best";
    const string MAX_ID_NODE_NAME               = "max";
    const string TOXICITY_ALARM_NODE_NAME       = "toxicityAlarm";

    string activeMoietyID = getChildStringValue(_targetDataRootIterator, ANALYTE_ID_NODE_NAME);
    string targetType = getChildStringValue(_targetDataRootIterator, TARGET_TYPE_NODE_NAME);
    string unit = getChildStringValue(_targetDataRootIterator, UNIT_NODE_NAME);
    double inefficacyAlarm = getChildDoubleValue(_targetDataRootIterator, INEFFICACY_ALARM_ID_NODE_NAME);
    double min = getChildDoubleValue(_targetDataRootIterator, MIN_NODE_NAME);
    double best= getChildDoubleValue(_targetDataRootIterator, BEST_NODE_NAME);
    double max = getChildDoubleValue(_targetDataRootIterator, MAX_ID_NODE_NAME);
    double toxicityAlarm = getChildDoubleValue(_targetDataRootIterator, TOXICITY_ALARM_NODE_NAME);

    return make_unique<TargetData>(
                               activeMoietyID,
                               targetType,
                               unit,
                               inefficacyAlarm,
                               min,
                               best,
                               max,
                               toxicityAlarm
                        );
}

unique_ptr<SampleData> QueryImport::createSampleData(Common::XmlNodeIterator& _sampleDataRootIterator) const
{
    const string SAMPLE_ID_NODE_NAME = "sampleID";
    const string SAMPLE_DATE_NODE_NAME = "sampleDate";
    const string ARRIVAL_DATE_NODE_NAME = "arrivalDate";
    const string CONCENTRATIONS_NODE_NAME = "concentrations";
    const string LIKELYHOOD_USE_NODE_NAME = "likelyhoodUse";

    string sampleID = getChildStringValue(_sampleDataRootIterator, SAMPLE_ID_NODE_NAME);
    Common::DateTime sampleDate = getChildDateTimeValue(_sampleDataRootIterator, SAMPLE_DATE_NODE_NAME);
    Common::DateTime arrivalDate = getChildDateTimeValue(_sampleDataRootIterator, ARRIVAL_DATE_NODE_NAME);

    vector< unique_ptr<ConcentrationData> > concentrations;

    Common::XmlNodeIterator concentrationsRootIterator = _sampleDataRootIterator->getChildren(CONCENTRATIONS_NODE_NAME);
    Common::XmlNodeIterator concentrationsIterator = concentrationsRootIterator->getChildren();
    while(concentrationsIterator != concentrationsIterator.none()) {
        concentrations.push_back(createConcentrationData(concentrationsIterator));
        concentrationsIterator++;
    }

    bool likelyhoodUse = getChildBoolValue(_sampleDataRootIterator, LIKELYHOOD_USE_NODE_NAME);

    return make_unique<SampleData>(
                               sampleID,
                               sampleDate,
                               arrivalDate,
                               concentrations,
                               likelyhoodUse
                        );
}

unique_ptr<ConcentrationData> QueryImport::createConcentrationData(Common::XmlNodeIterator& _concentrationDataRootIterator) const
{
    const string ANALITE_ID_NODE_NAME = "analyteID";
    const string VALUE_NODE_NAME = "value";
    const string UNIT_NODE_NAME = "unit";

    string analyteID = getChildStringValue(_concentrationDataRootIterator, ANALITE_ID_NODE_NAME);
    double value = getChildDoubleValue(_concentrationDataRootIterator, VALUE_NODE_NAME);
    string unit = getChildStringValue(_concentrationDataRootIterator, UNIT_NODE_NAME);

    return make_unique<ConcentrationData>(analyteID, value, unit);
}

unique_ptr<Treatment> QueryImport::createTreatment(Common::XmlNodeIterator& _treatmentRootIterator) const
{
    const string DOSAGE_HISTORY_NODE_NAME = "dosageHistory";

    Common::XmlNodeIterator dosageHistoryRootIterator = _treatmentRootIterator->getChildren(DOSAGE_HISTORY_NODE_NAME);
    Common::XmlNodeIterator dosageTimeRangeIterator = dosageHistoryRootIterator->getChildren();

    unique_ptr<Core::DosageHistory> pDosageHistory = make_unique<Core::DosageHistory>();
    while(dosageTimeRangeIterator != dosageTimeRangeIterator.none()) {
        unique_ptr<Core::DosageTimeRange> pDosageTimeRange = createDosageTimeRange(dosageTimeRangeIterator);
        pDosageHistory->addTimeRange(*pDosageTimeRange);
        dosageTimeRangeIterator++;
    }

    return make_unique<Treatment>(move(pDosageHistory));
}

unique_ptr<Core::DosageTimeRange> QueryImport::createDosageTimeRange(Common::XmlNodeIterator& _dosageTimeRangeRootIterator) const
{
    const string START_NODE_NAME    = "start";
    const string END_NODE_NAME      = "end";
    const string DOSAGE_NODE_NAME   = "dosage";

    Common::DateTime start = getChildDateTimeValue(_dosageTimeRangeRootIterator, START_NODE_NAME);
    Common::DateTime end = getChildDateTimeValue(_dosageTimeRangeRootIterator, END_NODE_NAME);

    Common::XmlNodeIterator dosageRootIterator = _dosageTimeRangeRootIterator->getChildren(DOSAGE_NODE_NAME);
    unique_ptr<Core::Dosage> pDosage = createDosage(dosageRootIterator);
    return make_unique<Core::DosageTimeRange>(
                                            start,
                                            end,
                                            *pDosage
                                        );
}

unique_ptr<Core::Dosage> QueryImport::createDosage(Common::XmlNodeIterator& _dosageRootIterator) const
{
    const string DOSAGE_LOOP_NODE_NAME  = "dosageLoop";

    Common::XmlNodeIterator dosageIterator = _dosageRootIterator->getChildren();
    if (dosageIterator->getName() == DOSAGE_LOOP_NODE_NAME) {
        // Create dosage loop from a bounded dosage
        unique_ptr<Core::DosageBounded> pDosageBounded = createDosageBounded(dosageIterator);
        return make_unique<Core::DosageLoop>(*pDosageBounded);
    } else {
        // Create bounded dosage
        // For this case we give the same root iterator as the one given as parameter for this method
        return createDosageBounded(_dosageRootIterator);
    }
}

unique_ptr<Core::DosageBounded> QueryImport::createDosageBounded(Common::XmlNodeIterator& _dosageBoundedRootIterator) const
{
    const string DOSAGE_REPEAT_NODE_NAME            = "dosageRepeat";
    const string DOSAGE_SEQUENCE_NODE_NAME          = "dosageSequence";
    const string LASTING_DOSAGE_NODE_NAME           = "lastingDosage";
    const string DAILY_DOSAGE_NODE_NAME             = "dailyDosage";
    const string WEEKLY_DOSAGE_NODE_NAME            = "weeklyDosage";
    const string DOSE_NODE_NAME                     = "dose";
    const string DOSE_VALUE_NODE_NAME               = "value";
    const string DOSE_INFUSIONTIME_NAME             = "infusionTimeInMinutes";
    const string FORMULATION_AND_ROUTE_NODE_NAME    = "formulationAndRoute";

    unique_ptr<Core::DosageBounded> pDosageBounded;
    Common::XmlNodeIterator dosageBoundedIterator = _dosageBoundedRootIterator->getChildren();
    if (dosageBoundedIterator->getName() == DOSAGE_REPEAT_NODE_NAME) {
        const string ITERATIONS_NODE_NAME = "iterations";

        int iterations = getChildIntValue(dosageBoundedIterator, ITERATIONS_NODE_NAME);
        unique_ptr<Core::DosageBounded> pDosageBounded = createDosageBounded(dosageBoundedIterator);
        pDosageBounded = make_unique<Core::DosageRepeat>(*pDosageBounded, iterations);
    } else if (dosageBoundedIterator->getName() == DOSAGE_SEQUENCE_NODE_NAME) {
        Core::DosageBoundedList dosageBoundedList;
        Common::XmlNodeIterator dosageSequenceIterator = dosageBoundedIterator->getChildren();
        while(dosageSequenceIterator != dosageSequenceIterator.none()) {
         dosageBoundedList.push_back(createDosageBounded(dosageSequenceIterator));
         dosageSequenceIterator++;
        }

        pDosageBounded = make_unique<Core::DosageSequence>(*(dosageBoundedList.at(0)));
    } else if (dosageBoundedIterator->getName() == LASTING_DOSAGE_NODE_NAME) {
        const string INTERVAL_NODE_NAME                 = "interval";


        string intervalValue = dosageBoundedIterator->getChildren(INTERVAL_NODE_NAME)->getValue();
        Common::TimeOfDay inter = Common::DateTime(intervalValue, "%H:%M:%S").getTimeOfDay();
        Common::Duration interval =inter.getRealDuration();

        Common::XmlNodeIterator doseRootIterator = dosageBoundedIterator->getChildren(DOSE_NODE_NAME);
        Core::DoseValue doseValue = getChildDoubleValue(doseRootIterator, DOSE_VALUE_NODE_NAME);
        double infuTimeInMinutes = getChildDoubleValue(doseRootIterator, DOSE_INFUSIONTIME_NAME);
        Common::Duration infusionTime = Duration(std::chrono::minutes(static_cast<int>(infuTimeInMinutes)));

        Common::XmlNodeIterator formulationAndRouteRootIterator = dosageBoundedIterator->getChildren(FORMULATION_AND_ROUTE_NODE_NAME);
        unique_ptr<Core::FormulationAndRoute> formulationAndRoute = createFormulationAndRoute(formulationAndRouteRootIterator);

        pDosageBounded = make_unique<Core::LastingDose>(
                                                           doseValue,
                                                           *formulationAndRoute,
                                                           infusionTime,
                                                           interval
                                                    );
    } else if (dosageBoundedIterator->getName() == DAILY_DOSAGE_NODE_NAME) {
        const string TIME_NODE_NAME = "time";

        string timeValue = dosageBoundedIterator->getChildren(TIME_NODE_NAME)->getValue();
        Common::TimeOfDay time(Common::DateTime(timeValue, "%H:%M:%S").getTimeOfDay());

        Common::XmlNodeIterator doseRootIterator = dosageBoundedIterator->getChildren(DOSE_NODE_NAME);
        Core::DoseValue doseValue = getChildDoubleValue(doseRootIterator, DOSE_VALUE_NODE_NAME);

        Common::XmlNodeIterator formulationAndRouteRootIterator = dosageBoundedIterator->getChildren(FORMULATION_AND_ROUTE_NODE_NAME);
        unique_ptr<Core::FormulationAndRoute> formulationAndRoute = createFormulationAndRoute(formulationAndRouteRootIterator);

        double infuTimeInMinutes = getChildDoubleValue(doseRootIterator, DOSE_INFUSIONTIME_NAME);
        Common::Duration infusionTime = Duration(std::chrono::minutes(static_cast<int>(infuTimeInMinutes)));

        pDosageBounded = make_unique<Core::DailyDose>(
                                                       doseValue,
                                                       *formulationAndRoute,
                                                       infusionTime,
                                                       time
                                                       );
    } else if (dosageBoundedIterator->getName() == WEEKLY_DOSAGE_NODE_NAME) {
        const string DAY_NODE_NAME = "day";
        const string TIME_NODE_NAME = "time";

        unsigned int day = static_cast<unsigned int>(getChildIntValue(dosageBoundedIterator, DAY_NODE_NAME));

        if (day > SATURDAY) {
            // TODO throw or manage error
            day = day % 7;
        }

        string timeValue = dosageBoundedIterator->getChildren(TIME_NODE_NAME)->getValue();
        Common::TimeOfDay time(Common::DateTime(timeValue, "%H:%M:%S").getTimeOfDay());

        Common::XmlNodeIterator doseRootIterator = dosageBoundedIterator->getChildren(DOSE_NODE_NAME);
        Core::DoseValue doseValue = getChildDoubleValue(doseRootIterator, DOSE_VALUE_NODE_NAME);

        Common::XmlNodeIterator formulationAndRouteRootIterator = dosageBoundedIterator->getChildren(FORMULATION_AND_ROUTE_NODE_NAME);
        unique_ptr<Core::FormulationAndRoute> formulationAndRoute = createFormulationAndRoute(formulationAndRouteRootIterator);

        double infuTimeInMinutes = getChildDoubleValue(doseRootIterator, DOSE_INFUSIONTIME_NAME);
        Common::Duration infusionTime = Duration(std::chrono::minutes(static_cast<int>(infuTimeInMinutes)));

        pDosageBounded = make_unique<Core::WeeklyDose>(
                                                     doseValue,
                                                     *formulationAndRoute,
                                                     infusionTime,
                                                     time,
                                                     Core::DayOfWeek(static_cast<unsigned>(day))
                                                    );
    }

    return pDosageBounded;
}

unique_ptr<Core::FormulationAndRoute> QueryImport::createFormulationAndRoute(Common::XmlNodeIterator &_formulationAndRouteRootIterator) const
{
    const string FORMULATION_NODE_NAME          = "formulation";
    const string ADMINISTRATION_NAME_NODE_NAME  = "administrationName";
    const string ADMINISTRATION_ROUTE_NODE_NAME = "administrationRoute";
    const string ABSORBTION_MODEL_NODE_NAME     = "absorbtionModel";

    string formulationValue = getChildStringValue(_formulationAndRouteRootIterator, FORMULATION_NODE_NAME);
    Core::Formulation formulation = Core::Formulation::Undefined;

    if (formulationValue == "Undefined") {
        formulation = Core::Formulation::Undefined;
    } else if (formulationValue == "OralSolution") {
        formulation = Core::Formulation::OralSolution;
    } else if (formulationValue == "ParenteralSolution") {
        formulation = Core::Formulation::ParenteralSolution;
    } else if (formulationValue == "Test") {
        formulation = Core::Formulation::Test;
    } else {
        // Throw error or manage error
        formulation = Core::Formulation::Undefined;
    }

    string administrationName = getChildStringValue(_formulationAndRouteRootIterator, ADMINISTRATION_NAME_NODE_NAME);
    string administrationRouteValue = getChildStringValue(_formulationAndRouteRootIterator, ADMINISTRATION_ROUTE_NODE_NAME);
    Core::AdministrationRoute administrationRoute = Core::AdministrationRoute::Undefined;

    if (administrationRouteValue == "Undefined") {
        administrationRoute = Core::AdministrationRoute::Undefined;
    } else if (administrationRouteValue == "Intramuscular") {
        administrationRoute = Core::AdministrationRoute::Intramuscular;
    } else if (administrationRouteValue == "IntravenousBolus") {
        administrationRoute = Core::AdministrationRoute::IntravenousBolus;
    } else if (administrationRouteValue == "IntravenousDrip") {
        administrationRoute = Core::AdministrationRoute::IntravenousDrip;
    } else if (administrationRouteValue == "Nasal") {
        administrationRoute = Core::AdministrationRoute::Nasal;
    } else if (administrationRouteValue == "Oral") {
        administrationRoute = Core::AdministrationRoute::Oral;
    } else if (administrationRouteValue == "Rectal") {
        administrationRoute = Core::AdministrationRoute::Rectal;
    } else if (administrationRouteValue == "Subcutaneous") {
        administrationRoute = Core::AdministrationRoute::Subcutaneous;
    } else if (administrationRouteValue == "Sublingual") {
        administrationRoute = Core::AdministrationRoute::Sublingual;
    } else if (administrationRouteValue == "Transdermal") {
        administrationRoute = Core::AdministrationRoute::Transdermal;
    } else if (administrationRouteValue == "Vaginal") {
        administrationRoute = Core::AdministrationRoute::Vaginal;
    } else {
        // Throw error or manage error
        formulation = Core::Formulation::Undefined;
    }

    string absorbtionModelValue = getChildStringValue(_formulationAndRouteRootIterator, ABSORBTION_MODEL_NODE_NAME);
    Core::AbsorptionModel absorbtionModel = Core::AbsorptionModel::UNDEFINED;

    if (absorbtionModelValue == "Undefined") {
        absorbtionModel = Core::AbsorptionModel::UNDEFINED;
    } else if (absorbtionModelValue == "Extravascular") {
        absorbtionModel = Core::AbsorptionModel::EXTRAVASCULAR;
    } else if (absorbtionModelValue == "Intravascular") {
        absorbtionModel = Core::AbsorptionModel::INTRAVASCULAR;
    } else if (absorbtionModelValue == "Infusion") {
        absorbtionModel = Core::AbsorptionModel::INFUSION;
    } else {
        // Throw error or manage error
        absorbtionModel = Core::AbsorptionModel::UNDEFINED;
    }

    return make_unique<Core::FormulationAndRoute>(
                                               formulation,
                                               administrationRoute,
                                               absorbtionModel,
                                               administrationName
                                            );
}

unique_ptr<RequestData> QueryImport::createRequest(Tucuxi::Common::XmlNodeIterator& _requestRootIterator) const
{
    const string REQUEST_ID_NODE_NAME           = "requestID";
    const string DRUG_ID_NODE_NAME              = "drugID";
    const string REQUEST_TYPE_NODE_NAME         = "requestType";
    const string NBPOINTSPERHOUR_NODE_NAME         = "nbPointsPerHour";
    const string DATE_INTERVAL_NODE_NAME        = "dateInterval";
    const string DATE_INTERVAL_START_NODE_NAME  = "start";
    const string DATE_INTERVAL_END_NODE_NAME    = "end";
    const string PREDICTION_TYPE_NODE_NAME      = "predictionType";
    const string GRAPH_NODE_NAME                = "graph";
    const string PERCENTILES_NODE_NAME          = "percentiles";
    const string BACKEXTRAPOLATION_NODE_NAME    = "backextrapolation";

    string requestID = getChildStringValue(_requestRootIterator, REQUEST_ID_NODE_NAME);
    string drugID = getChildStringValue(_requestRootIterator, DRUG_ID_NODE_NAME);
    string requestType = getChildStringValue(_requestRootIterator, REQUEST_TYPE_NODE_NAME);

    Common::XmlNodeIterator dateIntervalRootIterator = _requestRootIterator->getChildren(DATE_INTERVAL_NODE_NAME);
    Common::DateTime start = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_START_NODE_NAME);
    Common::DateTime end = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_END_NODE_NAME);
    unique_ptr<DateInterval> dateInterval = make_unique<DateInterval>(start, end);

    int nbPointsPerHour = getChildIntValue(_requestRootIterator, NBPOINTSPERHOUR_NODE_NAME);

    string predictionType = getChildStringValue(_requestRootIterator, PREDICTION_TYPE_NODE_NAME);

    Common::XmlNodeIterator graphRootIterator = _requestRootIterator->getChildren(GRAPH_NODE_NAME);
    unique_ptr<GraphData> pGraphData = createGraphData(graphRootIterator);

    Common::XmlNodeIterator percentilesRootIterator = _requestRootIterator->getChildren(PERCENTILES_NODE_NAME);
    Common::XmlNodeIterator percentilesIterator = percentilesRootIterator->getChildren();
    vector<double> percentiles;
    while(percentilesIterator != percentilesIterator.none()) {
        string percentileValue = percentilesIterator->getValue();
        double percentile = 0;
        try {
            percentile = static_cast<double>(std::stod(percentileValue));
        } catch (invalid_argument e) {
            percentile = 0;
        } catch (out_of_range e) {
            percentile = 0;
        }
        percentiles.push_back(percentile);
        percentilesIterator++;
    }

    Common::XmlNodeIterator backextrapolationRootIterator = _requestRootIterator->getChildren(BACKEXTRAPOLATION_NODE_NAME);
    unique_ptr<Backextrapolation> pBackextrapolation = createBackextrapolation(backextrapolationRootIterator);

    return make_unique<RequestData>(
                                 requestID,
                                 drugID,
                                 requestType,
                                 nbPointsPerHour,
                                 move(dateInterval),
                                 predictionType,
                                 move(pGraphData),
                                 percentiles,
                                 move(pBackextrapolation)
                             );
}

unique_ptr<GraphData> QueryImport::createGraphData(Common::XmlNodeIterator& _graphDataRootIterator) const
{
    const string START_NODE_NAME        = "start";
    const string END_NODE_NAME          = "end";
    const string PERCENTILES_NODE_NAME  = "percentiles";

    Common::DateTime start = getChildDateTimeValue(_graphDataRootIterator, START_NODE_NAME);
    Common::DateTime end = getChildDateTimeValue(_graphDataRootIterator, END_NODE_NAME);

    vector<unsigned short> percentiles;
    Common::XmlNodeIterator percentilesRootIterator = _graphDataRootIterator->getChildren(PERCENTILES_NODE_NAME);
    Common::XmlNodeIterator percentilesIterator = percentilesRootIterator->getChildren();
    while(percentilesIterator != percentilesIterator.none()) {
        string value = percentilesIterator->getValue();

        unsigned short finalValue = 0;
        try {
            finalValue = static_cast<unsigned short>(stoul(value));
        } catch (invalid_argument e) {
            finalValue = 0;
        } catch (out_of_range e) {
            finalValue = 0;
        }

        percentiles.push_back(finalValue);
        percentilesIterator++;
    }

    unique_ptr<DateInterval> pDateInterval = make_unique<DateInterval>(start, end);
    return make_unique<GraphData>(move(pDateInterval), percentiles);
}

unique_ptr<Backextrapolation> QueryImport::createBackextrapolation(Common::XmlNodeIterator& _backextrapolationRootIterator) const
{
    const string SAMPLE_NODE_NAME               = "sample";
    const string INCOMPLETE_SAMPLE_NODE_NAME    = "incompleteSample";
    const string DOSAGE_NODE_NAME               = "dosage";

    Common::XmlNodeIterator sampleRootIterator = _backextrapolationRootIterator->getChildren(SAMPLE_NODE_NAME);
    unique_ptr<SampleData> pSampleData = createSampleData(sampleRootIterator);

    Common::XmlNodeIterator dosageRootIterator = _backextrapolationRootIterator->getChildren(DOSAGE_NODE_NAME);
    //unique_ptr<Core::Dosage> pDosage = createDosage(dosageRootIterator, );
    // TODO

    // WARNING! nullptr for now
    unique_ptr<Backextrapolation> pBackextrapolation;
    return pBackextrapolation;
}

const string QueryImport::getChildStringValue(Common::XmlNodeIterator _rootIterator, string _childName) const
{
     return _rootIterator->getChildren(_childName)->getValue();
}

int QueryImport::getChildIntValue(Common::XmlNodeIterator _rootIterator, string _childName) const
{
     string value = _rootIterator->getChildren(_childName)->getValue();
     int finalValue = 0;
     try {
         finalValue = stoi(value);
     } catch (invalid_argument e) {
         finalValue = 0;
     } catch (out_of_range e) {
         finalValue = 0;
     }

     return finalValue;
}

double QueryImport::getChildDoubleValue(Common::XmlNodeIterator _rootIterator, string _childName) const
{
    string value = _rootIterator->getChildren(_childName)->getValue();
    double finalValue = 0.0;
    try {
        finalValue = stod(value);
    } catch (invalid_argument e) {
        finalValue = 0.0;
    } catch (out_of_range e) {
        finalValue = 0.0;
    }

    return finalValue;
}

bool QueryImport::getChildBoolValue(Common::XmlNodeIterator _rootIterator, string _childName) const
{
    string value = _rootIterator->getChildren(_childName)->getValue();
    boost::to_lower(value);
    bool finalValue = false;

    if (value == "true") {
        finalValue = true;
    } else if (value == "false") {
        finalValue = false;
    } else {
        // TODO
    }

    return finalValue;
}

Common::DateTime QueryImport::getChildDateTimeValue(Common::XmlNodeIterator _rootIterator, string _childName) const
{
    string value = _rootIterator->getChildren(_childName)->getValue();
    Common::DateTime datetime(value, m_sDATE_FORMAT);
    return datetime;
}

} // namespace Query
} // namespace Tucuxi
