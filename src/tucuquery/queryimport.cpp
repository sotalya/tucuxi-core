#include "queryimport.h"

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/utils.h"


// #include <unistd.h>
#include <chrono>

#include "tucucore/definitions.h"

using namespace std;

namespace Tucuxi {
namespace Query {


static const std::string DATE_FORMAT = "%Y-%m-%dT%H:%M:%S"; // NOLINT(readability-identifier-naming)


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

    static const string DRUG_ID_NODE_NAME = "queryId";
    static const string CLIENT_ID_NODE_NAME = "clientId";
    static const string DATE_NODE_NAME = "date";
    static const string LANGUAGE_NODE_NAME = "language";
    static const string REQUESTS_NODE_NAME = "requests";

    Common::XmlNode root = _document.getRoot();

    string drugId = root.getChildren(DRUG_ID_NODE_NAME)->getValue();
    string clientId = root.getChildren(CLIENT_ID_NODE_NAME)->getValue();

    string dateValue = root.getChildren(DATE_NODE_NAME)->getValue();
    Common::DateTime date(dateValue, DATE_FORMAT);

    string language = root.getChildren(LANGUAGE_NODE_NAME)->getValue();

    unique_ptr<AdministrativeData> pAdministrativeData = createAdministrativeData(_document);
    unique_ptr<DrugTreatmentData> pParametersData = createDrugTreatmentData(_document);

    Common::XmlNodeIterator requestsRootIterator = root.getChildren(REQUESTS_NODE_NAME);
    Common::XmlNodeIterator requestsIterator = requestsRootIterator->getChildren();
    vector< unique_ptr<RequestData> > requests;
    while(requestsIterator != requestsIterator.none()) {
        requests.emplace_back(createRequest(requestsIterator));
        requestsIterator++;
    }

    _query = new Query(
                drugId,
                clientId,
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
    static const string ADMIN_NODE_NAME = "admin";
    static const string MANDATOR_NODE_NAME = "mandator";
    static const string PATIENT_NODE_NAME = "patient";
    static const string CLINICALDATA_NODE_NAME = "clinicalData";

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
    static const string PERSON_NODE_NAME = "person";
    static const string INSTITUTE_NODE_NAME = "institute";

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
    static const string ID_NODE_NAME = "id";
    static const string TITLE_NODE_NAME = "title";
    static const string FIRSTNAME_NODE_NAME = "firstName";
    static const string LASTNAME_NODE_NAME = "lastName";
    static const string ADDRESS_NODE_NAME = "address";
    static const string PHONE_NODE_NAME = "phone";
    static const string EMAIL_NODE_NAME = "email";

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
    static const string ID_NODE_NAME = "id";
    static const string NAME_NODE_NAME = "name";
    static const string ADDRESS_NODE_NAME = "address";
    static const string PHONE_NODE_NAME = "phone";
    static const string EMAIL_NODE_NAME = "email";

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
    static const string STREET_NODE_NAME = "street";
    static const string POSTALCODE_NODE_NAME = "postCode";
    static const string CITY_NODE_NAME = "city";
    static const string STATE_NODE_NAME = "state";
    static const string COUNTRY_NODE_NAME = "country";

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
    static const string NUMBER_NODE_NAME = "address";
    static const string TYPE_NODE_NAME = "type";

    string number = getChildStringValue(_phoneRootIterator, NUMBER_NODE_NAME);
    string type = getChildStringValue(_phoneRootIterator, TYPE_NODE_NAME);

    return make_unique<Phone>(number, type);
}

unique_ptr<Email> QueryImport::createEmail(Common::XmlNodeIterator& _emailRootIterator) const
{
    static const string ADDRESS_NODE_NAME = "address";
    static const string TYPE_NODE_NAME = "type";

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

unique_ptr<DrugTreatmentData> QueryImport::createDrugTreatmentData(Tucuxi::Common::XmlDocument & _document) const
{
    static const string DRUGTREATMENT_NODE_NAME = "drugTreatment";
    static const string PATIENT_NODE_NAME = "patient";
    static const string DRUGS_NODE_NAME = "drugs";

    Common::XmlNode root = _document.getRoot();
    Common::XmlNodeIterator parametersRootIterator = root.getChildren(DRUGTREATMENT_NODE_NAME);

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

    return make_unique<DrugTreatmentData>(
                                      move(pPatient),
                                      move(drugs)
                                    );
}

unique_ptr<PatientData> QueryImport::createPatientData(Common::XmlNodeIterator& _patientDataRootIterator) const
{
    static const string COVARIATES_NODE_NAME = "covariates";
    static const string COVARIATE_NODE_NAME = "covariate";

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
    static const string COVARIATEID_NODE_NAME = "covariateId";
    static const string DATE_NODE_NAME = "date";
    static const string VALUE_NODE_NAME = "value";
    static const string UNIT_NODE_NAME = "unit";
    static const string DATATYPE_NODE_NAME = "dataType";
    static const string NATURE_NODE_NAME = "nature";

    string covariateId = getChildStringValue(_covariateDataRootIterator, COVARIATEID_NODE_NAME);
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
        // TODO : there is an error to notify
        dataType = Core::DataType::Double;
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
                                    covariateId,
                                     date,
                                     value,
                                     unit,
                                     dataType,
                                     nature
                                );
}


struct CmpByDate
{
    inline bool operator()(const std::unique_ptr<SampleData>& _a, const std::unique_ptr<SampleData>& _b)
    {
        return _a->getpSampleDate() < _b->getpSampleDate();
    }
};

unique_ptr<DrugData> QueryImport::createDrugData(Common::XmlNodeIterator& _drugDataRootIterator) const
{
    static const string DRUG_ID_NODE_NAME = "drugId";
    static const string DRUG_MODEL_ID_NODE_NAME = "drugModelId";
    static const string ACTIVE_PRINCIPLE_NODE_NAME = "activePrinciple";
    static const string BRAND_NAME_NODE_NAME = "brandName";
    static const string ATC_NODE_NAME = "atc";
    static const string TREATMENT_NODE_NAME = "treatment";
    static const string SAMPLES_NODE_NAME = "samples";
    static const string TARGETS_NODE_NAME = "targets";

    string drugId = getChildStringValue(_drugDataRootIterator, DRUG_ID_NODE_NAME);
    string drugModelId = getChildStringValue(_drugDataRootIterator, DRUG_MODEL_ID_NODE_NAME);
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


    sort(samples.begin(), samples.end(), CmpByDate());


    vector< unique_ptr<TargetData> > targets;
    Common::XmlNodeIterator targetsRootIterator = _drugDataRootIterator->getChildren(TARGETS_NODE_NAME);
    Common::XmlNodeIterator targetsIterator = targetsRootIterator->getChildren();
    while(targetsIterator != targetsIterator.none()) {
        targets.push_back(createTargetData(targetsIterator));
        targetsIterator++;
    }

    return make_unique<DrugData>(
                            drugId,
                            drugModelId,
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
    static const string ANALYTE_ID_NODE_NAME           = "activeMoietyId";
    static const string TARGET_TYPE_NODE_NAME          = "targetType";
    static const string UNIT_NODE_NAME                 = "unit";
    static const string INEFFICACY_ALARM_ID_NODE_NAME  = "inefficacyAlarm";
    static const string MIN_NODE_NAME                  = "min";
    static const string BEST_NODE_NAME                 = "best";
    static const string MAX_ID_NODE_NAME               = "max";
    static const string TOXICITY_ALARM_NODE_NAME       = "toxicityAlarm";

    string activeMoietyId = getChildStringValue(_targetDataRootIterator, ANALYTE_ID_NODE_NAME);
    string targetType = getChildStringValue(_targetDataRootIterator, TARGET_TYPE_NODE_NAME);
    string unit = getChildStringValue(_targetDataRootIterator, UNIT_NODE_NAME);
    double inefficacyAlarm = getChildDoubleValue(_targetDataRootIterator, INEFFICACY_ALARM_ID_NODE_NAME);
    double min = getChildDoubleValue(_targetDataRootIterator, MIN_NODE_NAME);
    double best= getChildDoubleValue(_targetDataRootIterator, BEST_NODE_NAME);
    double max = getChildDoubleValue(_targetDataRootIterator, MAX_ID_NODE_NAME);
    double toxicityAlarm = getChildDoubleValue(_targetDataRootIterator, TOXICITY_ALARM_NODE_NAME);

    return make_unique<TargetData>(
                               activeMoietyId,
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
    static const string SAMPLE_ID_NODE_NAME = "sampleId";
    static const string SAMPLE_DATE_NODE_NAME = "sampleDate";
    static const string ARRIVAL_DATE_NODE_NAME = "arrivalDate";
    static const string CONCENTRATIONS_NODE_NAME = "concentrations";
    static const string LIKELYHOOD_USE_NODE_NAME = "likelyhoodUse";

    string sampleId = getChildStringValue(_sampleDataRootIterator, SAMPLE_ID_NODE_NAME);
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
                               sampleId,
                               sampleDate,
                               arrivalDate,
                               concentrations,
                               likelyhoodUse
                        );
}

unique_ptr<ConcentrationData> QueryImport::createConcentrationData(Common::XmlNodeIterator& _concentrationDataRootIterator) const
{
    static const string ANALYTE_ID_NODE_NAME = "analyteId";
    static const string VALUE_NODE_NAME = "value";
    static const string UNIT_NODE_NAME = "unit";

    string analyteId = getChildStringValue(_concentrationDataRootIterator, ANALYTE_ID_NODE_NAME);
    double value = getChildDoubleValue(_concentrationDataRootIterator, VALUE_NODE_NAME);
    string unit = getChildStringValue(_concentrationDataRootIterator, UNIT_NODE_NAME);

    return make_unique<ConcentrationData>(analyteId, value, unit);
}

unique_ptr<Treatment> QueryImport::createTreatment(Common::XmlNodeIterator& _treatmentRootIterator) const
{
    static const string DOSAGE_HISTORY_NODE_NAME = "dosageHistory";

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
    static const string START_NODE_NAME    = "start";
    static const string END_NODE_NAME      = "end";
    static const string DOSAGE_NODE_NAME   = "dosage";

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
    static const string DOSAGE_LOOP_NODE_NAME  = "dosageLoop";

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
    static const string DOSAGE_REPEAT_NODE_NAME            = "dosageRepeat";
    static const string DOSAGE_SEQUENCE_NODE_NAME          = "dosageSequence";
    static const string LASTING_DOSAGE_NODE_NAME           = "lastingDosage";
    static const string DAILY_DOSAGE_NODE_NAME             = "dailyDosage";
    static const string WEEKLY_DOSAGE_NODE_NAME            = "weeklyDosage";
    static const string DOSE_NODE_NAME                     = "dose";
    static const string DOSE_VALUE_NODE_NAME               = "value";
    static const string DOSE_INFUSIONTIME_NAME             = "infusionTimeInMinutes";
    static const string FORMULATION_AND_ROUTE_NODE_NAME    = "formulationAndRoute";

    unique_ptr<Core::DosageBounded> pDosageBounded;
    Common::XmlNodeIterator dosageBoundedIterator = _dosageBoundedRootIterator->getChildren();
    if (dosageBoundedIterator->getName() == DOSAGE_REPEAT_NODE_NAME) {
        static const string ITERATIONS_NODE_NAME = "iterations";

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
        static const string INTERVAL_NODE_NAME                 = "interval";


        // string intervalValue = dosageBoundedIterator->getChildren(INTERVAL_NODE_NAME)->getValue();
        // Common::TimeOfDay inter = Common::DateTime(intervalValue, "%H:%M:%S").getTimeOfDay();
        Common::Duration interval = getChildDuration(dosageBoundedIterator, INTERVAL_NODE_NAME);

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
        static const string TIME_NODE_NAME = "time";

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
        static const string DAY_NODE_NAME = "day";
        static const string TIME_NODE_NAME = "time";

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
    static const string FORMULATION_NODE_NAME          = "formulation";
    static const string ADMINISTRATION_NAME_NODE_NAME  = "administrationName";
    static const string ADMINISTRATION_ROUTE_NODE_NAME = "administrationRoute";
    static const string ABSORPTION_MODEL_NODE_NAME     = "absorptionModel";

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

    string absorptionModelValue = getChildStringValue(_formulationAndRouteRootIterator, ABSORPTION_MODEL_NODE_NAME);
    Core::AbsorptionModel absorptionModel = Core::AbsorptionModel::Undefined;

    if (absorptionModelValue == "Undefined") {
        absorptionModel = Core::AbsorptionModel::Undefined;
    } else if (absorptionModelValue == "Extravascular") {
        absorptionModel = Core::AbsorptionModel::Extravascular;
    } else if (absorptionModelValue == "ExtravascularLag") {
        absorptionModel = Core::AbsorptionModel::ExtravascularLag;
    } else if (absorptionModelValue == "Intravascular") {
        absorptionModel = Core::AbsorptionModel::Intravascular;
    } else if (absorptionModelValue == "Infusion") {
        absorptionModel = Core::AbsorptionModel::Infusion;
    } else {
        // Throw error or manage error
        absorptionModel = Core::AbsorptionModel::Undefined;
    }

    return make_unique<Core::FormulationAndRoute>(
                                               formulation,
                                               administrationRoute,
                                               absorptionModel,
                                               administrationName
                                            );
}

unique_ptr<RequestData> QueryImport::createRequest(Tucuxi::Common::XmlNodeIterator& _requestRootIterator) const
{
    static const string REQUEST_ID_NODE_NAME           = "requestId";
    static const string DRUG_ID_NODE_NAME              = "drugId";
    static const string REQUEST_TYPE_NODE_NAME         = "requestType";
    static const string NBPOINTSPERHOUR_NODE_NAME      = "nbPointsPerHour";
    static const string DATE_INTERVAL_NODE_NAME        = "dateInterval";
    static const string DATE_INTERVAL_START_NODE_NAME  = "start";
    static const string DATE_INTERVAL_END_NODE_NAME    = "end";
    static const string PREDICTION_TYPE_NODE_NAME      = "parametersType";
    static const string GRAPH_NODE_NAME                = "graph";
    static const string PERCENTILES_NODE_NAME          = "percentiles";
    static const string POINTSINTIME_NODE_NAME         = "pointsInTime";
    static const string BACKEXTRAPOLATION_NODE_NAME    = "backextrapolation";

    string requestId = getChildStringValue(_requestRootIterator, REQUEST_ID_NODE_NAME);
    string drugId = getChildStringValue(_requestRootIterator, DRUG_ID_NODE_NAME);
    string requestType = getChildStringValue(_requestRootIterator, REQUEST_TYPE_NODE_NAME);

    Common::XmlNodeIterator dateIntervalRootIterator = _requestRootIterator->getChildren(DATE_INTERVAL_NODE_NAME);
    Common::DateTime start = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_START_NODE_NAME);
    Common::DateTime end = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_END_NODE_NAME);
    unique_ptr<DateInterval> dateInterval = make_unique<DateInterval>(start, end);

    int nbPointsPerHour = getChildIntValue(_requestRootIterator, NBPOINTSPERHOUR_NODE_NAME);

    string parametersType = getChildStringValue(_requestRootIterator, PREDICTION_TYPE_NODE_NAME);

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

    Common::XmlNodeIterator pointsInTimeRootIterator = _requestRootIterator->getChildren(POINTSINTIME_NODE_NAME);
    Common::XmlNodeIterator pointsInTimeIterator = pointsInTimeRootIterator->getChildren();
    vector<DateTime> pointsInTime;
    while(pointsInTimeIterator != pointsInTimeIterator.none()) {
        string value = pointsInTimeIterator->getValue();
        Common::DateTime datetime(value, DATE_FORMAT);
        pointsInTime.push_back(datetime);
        pointsInTimeIterator++;
    }

    Common::XmlNodeIterator backextrapolationRootIterator = _requestRootIterator->getChildren(BACKEXTRAPOLATION_NODE_NAME);
    unique_ptr<Backextrapolation> pBackextrapolation = createBackextrapolation(backextrapolationRootIterator);

    return make_unique<RequestData>(
                                 requestId,
                                 drugId,
                                 requestType,
                                 nbPointsPerHour,
                                 move(dateInterval),
                                 parametersType,
                                 move(pGraphData),
                                 percentiles,
                                 move(pBackextrapolation),
                                 pointsInTime
                             );
}

unique_ptr<GraphData> QueryImport::createGraphData(Common::XmlNodeIterator& _graphDataRootIterator) const
{
    static const string START_NODE_NAME        = "start";
    static const string END_NODE_NAME          = "end";
    static const string PERCENTILES_NODE_NAME  = "percentiles";

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
    static const string SAMPLE_NODE_NAME               = "sample";
    static const string INCOMPLETE_SAMPLE_NODE_NAME    = "incompleteSample";
    static const string DOSAGE_NODE_NAME               = "dosage";

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

    transform(value.begin(), value.end(), value.begin(), ::tolower);

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
    // TODO If value is empty, then something went wrong
    Common::DateTime datetime(value, DATE_FORMAT);
    return datetime;
}

Common::Duration QueryImport::getChildDuration(Common::XmlNodeIterator _rootIterator, string _childName) const
{
    string s = _rootIterator->getChildren(_childName)->getValue();
    std::string delimiter = ":";

    size_t pos = 0;
    std::string token;

    std::vector<int> values;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        values.push_back(std::stoi(token));
        s.erase(0, pos + delimiter.length());
    }
    values.push_back(std::stoi(s));
    return Common::Duration(std::chrono::hours(values[0]), std::chrono::minutes(values[1]), std::chrono::seconds(values[2]));
}

} // namespace Query
} // namespace Tucuxi
