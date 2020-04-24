#include "queryimport.h"

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/utils.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodelimport.h"


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

QueryImport::Result QueryImport::importFromFile(Tucuxi::Query::QueryData *&_query, std::string _fileName)
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


QueryImport::Result QueryImport::importFromString(Tucuxi::Query::QueryData *&_query, std::string _xml)
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
        Tucuxi::Query::QueryData *&_query,
        Tucuxi::Common::XmlDocument & _document)
{

    // TODO try to see if I can catch a parsing exception.

    static const string DRUG_ID_NODE_NAME = "queryId";
    static const string CLIENT_ID_NODE_NAME = "clientId";
    static const string DATE_NODE_NAME = "date";
    static const string LANGUAGE_NODE_NAME = "language";
    static const string REQUESTS_NODE_NAME = "requests";
    static const string REQUEST_REQUESTS_NODE_NAME = "request";

    Common::XmlNode root = _document.getRoot();

    string drugId = root.getChildren(DRUG_ID_NODE_NAME)->getValue();
    string clientId = root.getChildren(CLIENT_ID_NODE_NAME)->getValue();

    string dateValue = root.getChildren(DATE_NODE_NAME)->getValue();
    Common::DateTime date(dateValue, DATE_FORMAT);

    string language = root.getChildren(LANGUAGE_NODE_NAME)->getValue();

    unique_ptr<AdministrativeData> pAdministrativeData = createAdministrativeData(_document);
    unique_ptr<DrugTreatmentData> pParametersData = createDrugTreatmentData(_document);

    Common::XmlNodeIterator requestsRootIterator = root.getChildren(REQUESTS_NODE_NAME);
    Common::XmlNodeIterator requestsIterator = requestsRootIterator->getChildren(REQUEST_REQUESTS_NODE_NAME);

    vector< unique_ptr<RequestData> > requests;
    while(requestsIterator != requestsIterator.none()) {
        requests.emplace_back(createRequest(requestsIterator));
        requestsIterator++;
    }


    _query = new QueryData(
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
    static const string CONCENTRATIONS_NODE_NAME = "concentrations";
    static const string LIKELYHOOD_USE_NODE_NAME = "likelyhoodUse";

    string sampleId = getChildStringValue(_sampleDataRootIterator, SAMPLE_ID_NODE_NAME);
    Common::DateTime sampleDate = getChildDateTimeValue(_sampleDataRootIterator, SAMPLE_DATE_NODE_NAME);

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
    } else if (formulationValue == "oralSolution") {
        formulation = Core::Formulation::OralSolution;
    } else if (formulationValue == "parenteralSolution") {
        formulation = Core::Formulation::ParenteralSolution;
    } else if (formulationValue == "test") {
        formulation = Core::Formulation::Test;
    } else {
        // Throw error or manage error
        formulation = Core::Formulation::Undefined;
    }

    string administrationName = getChildStringValue(_formulationAndRouteRootIterator, ADMINISTRATION_NAME_NODE_NAME);
    string administrationRouteValue = getChildStringValue(_formulationAndRouteRootIterator, ADMINISTRATION_ROUTE_NODE_NAME);
    Core::AdministrationRoute administrationRoute = Core::AdministrationRoute::Undefined;

    if (administrationRouteValue == "undefined") {
        administrationRoute = Core::AdministrationRoute::Undefined;
    } else if (administrationRouteValue == "intramuscular") {
        administrationRoute = Core::AdministrationRoute::Intramuscular;
    } else if (administrationRouteValue == "intravenousBolus") {
        administrationRoute = Core::AdministrationRoute::IntravenousBolus;
    } else if (administrationRouteValue == "intravenousDrip") {
        administrationRoute = Core::AdministrationRoute::IntravenousDrip;
    } else if (administrationRouteValue == "nasal") {
        administrationRoute = Core::AdministrationRoute::Nasal;
    } else if (administrationRouteValue == "oral") {
        administrationRoute = Core::AdministrationRoute::Oral;
    } else if (administrationRouteValue == "rectal") {
        administrationRoute = Core::AdministrationRoute::Rectal;
    } else if (administrationRouteValue == "subcutaneous") {
        administrationRoute = Core::AdministrationRoute::Subcutaneous;
    } else if (administrationRouteValue == "sublingual") {
        administrationRoute = Core::AdministrationRoute::Sublingual;
    } else if (administrationRouteValue == "transdermal") {
        administrationRoute = Core::AdministrationRoute::Transdermal;
    } else if (administrationRouteValue == "vaginal") {
        administrationRoute = Core::AdministrationRoute::Vaginal;
    } else {
        // Throw error or manage error
        formulation = Core::Formulation::Undefined;
    }

    string absorptionModelValue = getChildStringValue(_formulationAndRouteRootIterator, ABSORPTION_MODEL_NODE_NAME);
    Core::AbsorptionModel absorptionModel = Core::AbsorptionModel::Undefined;

    if (absorptionModelValue == "Undefined") {
        absorptionModel = Core::AbsorptionModel::Undefined;
    } else if (absorptionModelValue == "extravascular") {
        absorptionModel = Core::AbsorptionModel::Extravascular;
    } else if (absorptionModelValue == "extravascularLag") {
        absorptionModel = Core::AbsorptionModel::ExtravascularLag;
    } else if (absorptionModelValue == "intravascular") {
        absorptionModel = Core::AbsorptionModel::Intravascular;
    } else if (absorptionModelValue == "infusion") {
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

unique_ptr<RequestData> QueryImport::createRequest(Tucuxi::Common::XmlNodeIterator& _requestRootIterator)
{
    static const string REQUEST_ID_NODE_NAME                        = "requestId";
    static const string DRUG_ID_NODE_NAME                           = "drugId";
    static const string DRUGMODEL_ID_NODE_NAME                      = "drugModelId";
    static const string COMPUTING_TRAIT_ADJUSTMENT_NAME             = "adjustmentTraits";
    static const string COMPUTING_TRAIT_CONCENTRATION_NAME          = "predictionTraits";
    static const string COMPUTING_TRAIT_PERCENTILES_NAME            = "percentilesTraits";
    static const string COMPUTING_TRAIT_SINGLE_POINT_NAME           = "predictionAtTimesTraits";
    static const string COMPUTING_TRAIT_AT_MESURE_NAME              = "predictionAtSampleTimesTraits";
    static const string REQUEST_TYPE_NODE_NAME                      = "requestType";
    static const string NBPOINTSPERHOUR_NODE_NAME                   = "nbPointsPerHour";
    static const string DATE_INTERVAL_NODE_NAME                     = "dateInterval";
    static const string DATE_INTERVAL_START_NODE_NAME               = "start";
    static const string DATE_INTERVAL_END_NODE_NAME                 = "end";
    static const string PREDICTION_TYPE_NODE_NAME                   = "parametersType";
    static const string GRAPH_NODE_NAME                             = "graph";
    static const string PERCENTILES_NODE_NAME                       = "percentiles";
    static const string POINTSINTIME_NODE_NAME                      = "pointsInTime";
    static const string BACKEXTRAPOLATION_NODE_NAME                 = "backextrapolation";

    string requestId = getChildStringValue(_requestRootIterator, REQUEST_ID_NODE_NAME);
    string drugId = getChildStringValue(_requestRootIterator, DRUG_ID_NODE_NAME);
    string drugModelId = getChildStringValue(_requestRootIterator, DRUGMODEL_ID_NODE_NAME);

    unique_ptr<Tucuxi::Core::ComputingTrait> computingTrait;
    Common::XmlNodeIterator computingTraitAdjustmentRootIterator = _requestRootIterator->getChildren(COMPUTING_TRAIT_ADJUSTMENT_NAME);

    if(computingTraitAdjustmentRootIterator != Common::XmlNodeIterator::none())
    {
        computingTrait = getChildComputingTraitAdjustment(computingTraitAdjustmentRootIterator, requestId);
    }

    Common::XmlNodeIterator computingTraitConcentrationRootIterator = _requestRootIterator->getChildren(COMPUTING_TRAIT_CONCENTRATION_NAME);

    if(computingTraitConcentrationRootIterator != Common::XmlNodeIterator::none())
    {
        computingTrait = getChildComputingTraitConcentration(computingTraitConcentrationRootIterator, requestId);
    }

    Common::XmlNodeIterator computingTraitPercentilesRootIterator = _requestRootIterator->getChildren(COMPUTING_TRAIT_PERCENTILES_NAME);

    if(computingTraitPercentilesRootIterator != Common::XmlNodeIterator::none())
    {
        computingTrait = getChildComputingTraitPercentiles(computingTraitPercentilesRootIterator, requestId);
    }

    Common::XmlNodeIterator computingTraitSinglePointRootIterator = _requestRootIterator->getChildren(COMPUTING_TRAIT_SINGLE_POINT_NAME);

    if(computingTraitSinglePointRootIterator != Common::XmlNodeIterator::none())
    {
        computingTrait = getChildComputingTraitSinglePoints(computingTraitSinglePointRootIterator, requestId);
    }

    Common::XmlNodeIterator computingTraitAtMeasuresRootIterator = _requestRootIterator->getChildren(COMPUTING_TRAIT_AT_MESURE_NAME);

    if(computingTraitAtMeasuresRootIterator != Common::XmlNodeIterator::none())
    {
        computingTrait = getChildComputingTraitAtMeasures(computingTraitAtMeasuresRootIterator, requestId);
    }

    if(computingTrait == nullptr)
    {
        setResult(Result::Error);
    }



    return make_unique<RequestData>(
                requestId,
                drugId,
                drugModelId,
                move(computingTrait)
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

Tucuxi::Core::PercentileRanks QueryImport::getChildPercentileRanks(Common::XmlNodeIterator _rootIterator, string _childName) const
{
    Common::XmlNodeIterator it = _rootIterator->getChildren(_childName);
    Tucuxi::Core::PercentileRanks ranks;
    Tucuxi::Core::PercentileRank rank;
    while(it != Common::XmlNodeIterator::none())
    {
        rank = getChildDoubleValue(_rootIterator, _childName);
        ranks.push_back(rank);
        it ++;
    }

    return ranks;
}

vector<Common::DateTime> QueryImport::getChildDateTimeList(Common::XmlNodeIterator _rootIterator, string _childName) const
{
    Common::XmlNodeIterator it = _rootIterator->getChildren(_childName);
    vector<DateTime> times;
    string value;
    while(it != Common::XmlNodeIterator::none())
    {
        value = it->getValue();
        Common::DateTime datetime(value, DATE_FORMAT);
        times.push_back(datetime);

        it ++;
    }

    return times;
}

unique_ptr<Tucuxi::Core::ComputingTraitAtMeasures> QueryImport::getChildComputingTraitAtMeasures(Common::XmlNodeIterator _rootIterator, string _requestResponseId)
{
    static const string COMPUTING_OPTION                = "computingOption";


    Tucuxi::Core::RequestResponseId requestResponseId = _requestResponseId;
    Tucuxi::Core::ComputingOption computingOption = getChildComputingOption(_rootIterator, COMPUTING_OPTION);

    return make_unique<Tucuxi::Core::ComputingTraitAtMeasures> (requestResponseId,
                                                                computingOption);
}


unique_ptr<Tucuxi::Core::ComputingTraitSinglePoints> QueryImport::getChildComputingTraitSinglePoints(Common::XmlNodeIterator _rootIterator, string _requestResponseId)
{
    static const string COMPUTING_OPTION                = "computingOption";
    static const string DATES_NODE_NAME                 = "dates";
    static const string DATES_DATE_NODE_NAME            = "date";

    Tucuxi::Core::RequestResponseId requestResponseId = _requestResponseId;

    Common::XmlNodeIterator timesRootIterator = _rootIterator->getChildren(DATES_NODE_NAME);
    vector<Common::DateTime> times = getChildDateTimeList(timesRootIterator, DATES_DATE_NODE_NAME);

    Tucuxi::Core::ComputingOption computingOption = getChildComputingOption(_rootIterator, COMPUTING_OPTION);

    return make_unique<Tucuxi::Core::ComputingTraitSinglePoints> (requestResponseId,
                                                                  times,
                                                                  computingOption);
}

unique_ptr<Tucuxi::Core::ComputingTraitPercentiles> QueryImport::getChildComputingTraitPercentiles(Common::XmlNodeIterator _rootIterator, string _requestResponseId)
{
    static const string NB_POINTS_PER_HOUR              = "nbPointsPerHour";
    static const string DATE_INTERVAL_NODE_NAME         = "dateInterval";
    static const string DATE_INTERVAL_START_NODE_NAME   = "start";
    static const string DATE_INTERVAL_END_NODE_NAME     = "end";
    static const string RANKS_NODE_NAME                 = "ranks";
    static const string RANKS_RANK_NODE_NAME            = "rank";
    static const string COMPUTING_OPTION                = "computingOption";

    Tucuxi::Core::RequestResponseId requestResponseId = _requestResponseId;

    Common::XmlNodeIterator dateIntervalRootIterator = _rootIterator->getChildren(DATE_INTERVAL_NODE_NAME);
    Common::DateTime start = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_START_NODE_NAME);
    Common::DateTime end = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_END_NODE_NAME);

    Common::XmlNodeIterator ranksRootIterator = _rootIterator->getChildren(RANKS_NODE_NAME);
    Tucuxi::Core::PercentileRanks ranks = getChildPercentileRanks(ranksRootIterator, RANKS_RANK_NODE_NAME);

    double nbPointsPerHour = getChildDoubleValue(_rootIterator, NB_POINTS_PER_HOUR);

    Tucuxi::Core::ComputingOption computingOption = getChildComputingOption(_rootIterator, COMPUTING_OPTION);

    return make_unique<Tucuxi::Core::ComputingTraitPercentiles> (requestResponseId,
                                                                 start,
                                                                 end,
                                                                 ranks,
                                                                 nbPointsPerHour,
                                                                 computingOption);
}

unique_ptr<Tucuxi::Core::ComputingTraitConcentration> QueryImport::getChildComputingTraitConcentration(Common::XmlNodeIterator _rootIterator, string _requestResponseId)
{
    static const string NB_POINTS_PER_HOUR              = "nbPointsPerHour";
    static const string DATE_INTERVAL_NODE_NAME         = "dateInterval";
    static const string DATE_INTERVAL_START_NODE_NAME   = "start";
    static const string DATE_INTERVAL_END_NODE_NAME     = "end";
    static const string COMPUTING_OPTION                = "computingOption";

    Tucuxi::Core::RequestResponseId requestResponseId = _requestResponseId;

    Common::XmlNodeIterator dateIntervalRootIterator = _rootIterator->getChildren(DATE_INTERVAL_NODE_NAME);
    Common::DateTime start = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_START_NODE_NAME);
    Common::DateTime end = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_END_NODE_NAME);

    double nbPointsPerHour = getChildDoubleValue(_rootIterator, NB_POINTS_PER_HOUR);

    Tucuxi::Core::ComputingOption computingOption = getChildComputingOption(_rootIterator, COMPUTING_OPTION);


    return make_unique<Tucuxi::Core::ComputingTraitConcentration> (requestResponseId,
                                                                   start,
                                                                   end,
                                                                   nbPointsPerHour,
                                                                   computingOption);
}

unique_ptr<Tucuxi::Core::ComputingTraitAdjustment> QueryImport::getChildComputingTraitAdjustment(Common::XmlNodeIterator _rootIterator, string _requestResponseId)
{
    static const string NB_POINTS_PER_HOUR              = "nbPointsPerHour";
    static const string DATE_INTERVAL_NODE_NAME         = "dateInterval";
    static const string DATE_INTERVAL_START_NODE_NAME   = "start";
    static const string DATE_INTERVAL_END_NODE_NAME     = "end";
    static const string COMPUTING_OPTION                = "computingOption";
    static const string DATE_ADJUSTMENT_TIME            = "adjustmentDate";
    static const string OPTIONS                         = "options";


    Tucuxi::Core::RequestResponseId requestResponseId = _requestResponseId;

    Common::XmlNodeIterator dateIntervalRootIterator = _rootIterator->getChildren(DATE_INTERVAL_NODE_NAME);
    Common::DateTime start = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_START_NODE_NAME);
    Common::DateTime end = getChildDateTimeValue(dateIntervalRootIterator, DATE_INTERVAL_END_NODE_NAME);

    double nbPointsPerHour = getChildDoubleValue(_rootIterator, NB_POINTS_PER_HOUR);

    Tucuxi::Core::ComputingOption computingOption = getChildComputingOption(_rootIterator, COMPUTING_OPTION);

    Common::DateTime adjustmentTime = getChildDateTimeValue(_rootIterator, DATE_ADJUSTMENT_TIME);

    Tucuxi::Core::BestCandidatesOption bestCandidateOption = getChildBestCandidatesOptionEnum(_rootIterator, OPTIONS);

    Tucuxi::Core::LoadingOption loadingOption = getChildLoadingOptionEnum(_rootIterator, OPTIONS);

    Tucuxi::Core::RestPeriodOption restPeriodOption = getChildRestPeriodTargetOptionEnum(_rootIterator, OPTIONS);

    Tucuxi::Core::SteadyStateTargetOption steadyStateTargetOption = getChildSteadyStateTargetOptionEnum(_rootIterator, OPTIONS);

    Tucuxi::Core::TargetExtractionOption targetExtractionOption = getChildTargetExtractionOptionEnum(_rootIterator, OPTIONS);

    Tucuxi::Core::FormulationAndRouteSelectionOption formulationAndRouteSelectionOption = getChildFormulationAndRouteSelectionOptionEnum(_rootIterator, OPTIONS);


    return make_unique<Tucuxi::Core::ComputingTraitAdjustment>(requestResponseId,
                                                               start,
                                                               end,
                                                               nbPointsPerHour,
                                                               computingOption,
                                                               adjustmentTime,
                                                               bestCandidateOption,
                                                               loadingOption,
                                                               restPeriodOption,
                                                               steadyStateTargetOption,
                                                               targetExtractionOption,
                                                               formulationAndRouteSelectionOption);

}

void QueryImport::setNodeError(Tucuxi::Common::XmlNodeIterator _node)
{
    std::string errorMessage;
    Tucuxi::Common::XmlNode node = _node->getParent();
    while (node.isValid()) {
        if (node.getName().size() != 0) {
            errorMessage = "<" + node.getName() + ">" + errorMessage;
        }
        node = node.getParent();
    }
    if (_node->getValue() == "") {
        errorMessage += "<" + _node->getName() + "> contains an empty value.";
    }
    else {
        errorMessage += "<" + _node->getName() + "> contains an invalid value : " + _node->getValue();
    }
    setResult(Result::Error, errorMessage);
}

Tucuxi::Core::BestCandidatesOption QueryImport::getChildBestCandidatesOptionEnum(Common::XmlNodeIterator _rootIterator, std::string _childName)
{

    static const string BEST_CANDIDATE_OPTION_NODE          = "bestCandidatesOption";

    Common::XmlNodeIterator optionsRootIterator = _rootIterator->getChildren(_childName);

    Common::XmlNodeIterator BestCandidatesOptionRootIterator = optionsRootIterator->getChildren(BEST_CANDIDATE_OPTION_NODE);

    static std::map<std::string,Tucuxi::Core::BestCandidatesOption> m =
    {
        {"bestDosage", Tucuxi::Core::BestCandidatesOption::BestDosage},
        {"allDosages", Tucuxi::Core::BestCandidatesOption::AllDosages},
        {"bestDosagePerInterval", Tucuxi::Core::BestCandidatesOption::BestDosagePerInterval}
    };

    string value = BestCandidatesOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(BestCandidatesOptionRootIterator);

    return Tucuxi::Core::BestCandidatesOption::BestDosage;
}

Tucuxi::Core::LoadingOption QueryImport::getChildLoadingOptionEnum(Common::XmlNodeIterator _rootIterator, std::string _childName)
{
    static const string LOADING_OPTION_NODE          = "loadingOption";

    Common::XmlNodeIterator optionsRootIterator = _rootIterator->getChildren(_childName);

    Common::XmlNodeIterator loadingOptionRootIterator = optionsRootIterator->getChildren(LOADING_OPTION_NODE);


    static std::map<std::string,Tucuxi::Core::LoadingOption> m =
    {
        {"noLoadingDose", Tucuxi::Core::LoadingOption::NoLoadingDose},
        {"loadingDoseAllowed", Tucuxi::Core::LoadingOption::LoadingDoseAllowed}
    };

    string value = loadingOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(loadingOptionRootIterator);
    return Tucuxi::Core::LoadingOption::NoLoadingDose;
}

Tucuxi::Core::RestPeriodOption QueryImport::getChildRestPeriodTargetOptionEnum(Common::XmlNodeIterator _rootIterator, std::string _childName)
{

    static const string REST_PERIOD_OPTION_NODE          = "restPeriodOption";

    Common::XmlNodeIterator optionsRootIterator = _rootIterator->getChildren(_childName);

    Common::XmlNodeIterator restPeriodOptionRootIterator = optionsRootIterator->getChildren(REST_PERIOD_OPTION_NODE);


    static std::map<std::string,Tucuxi::Core::RestPeriodOption> m =
    {
        {"noRestPeriod", Tucuxi::Core::RestPeriodOption::NoRestPeriod},
        {"restPeriodAllowed", Tucuxi::Core::RestPeriodOption::RestPeriodAllowed}
    };

    string value = restPeriodOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(restPeriodOptionRootIterator);
    return Tucuxi::Core::RestPeriodOption::NoRestPeriod;
}

Tucuxi::Core::SteadyStateTargetOption QueryImport::getChildSteadyStateTargetOptionEnum(Common::XmlNodeIterator _rootIterator, std::string _childName)
{

    static const string STEADY_STATE_TARGET_OPTION_NODE          = "steadyStateTargetOption";

    Common::XmlNodeIterator optionsRootIterator = _rootIterator->getChildren(_childName);

    Common::XmlNodeIterator steadyStateTargetOptionRootIterator = optionsRootIterator->getChildren(STEADY_STATE_TARGET_OPTION_NODE);

    static std::map<std::string,Tucuxi::Core::SteadyStateTargetOption> m =
    {
        {"atSteadyState", Tucuxi::Core::SteadyStateTargetOption::AtSteadyState},
        {"withinTreatmentTimeRange", Tucuxi::Core::SteadyStateTargetOption::WithinTreatmentTimeRange}
    };

    string value = steadyStateTargetOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(steadyStateTargetOptionRootIterator);
    return Tucuxi::Core::SteadyStateTargetOption::AtSteadyState;
}

Tucuxi::Core::TargetExtractionOption QueryImport::getChildTargetExtractionOptionEnum(Common::XmlNodeIterator _rootIterator, std::string _childName)
{

    static const string TARGET_EXTRACTION_OPTION_NODE          = "targetExtractionOption";

    Common::XmlNodeIterator optionsRootIterator = _rootIterator->getChildren(_childName);

    Common::XmlNodeIterator targetExtractionOptionRootIterator = optionsRootIterator->getChildren(TARGET_EXTRACTION_OPTION_NODE);


    static std::map<std::string,Tucuxi::Core::TargetExtractionOption> m =
    {
        {"populationValues", Tucuxi::Core::TargetExtractionOption::PopulationValues},
        {"aprioriValues", Tucuxi::Core::TargetExtractionOption::AprioriValues},
        {"individualTargets", Tucuxi::Core::TargetExtractionOption::IndividualTargets},
        {"individualTargetsIfDefinitionExists", Tucuxi::Core::TargetExtractionOption::IndividualTargetsIfDefinitionExists},
        {"definitionIfNoIndividualTarget", Tucuxi::Core::TargetExtractionOption::DefinitionIfNoIndividualTarget},
        {"individualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget", Tucuxi::Core::TargetExtractionOption::IndividualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget}

    };

    string value = targetExtractionOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(targetExtractionOptionRootIterator);
    return Tucuxi::Core::TargetExtractionOption::PopulationValues;
}

Tucuxi::Core::FormulationAndRouteSelectionOption QueryImport::getChildFormulationAndRouteSelectionOptionEnum(Common::XmlNodeIterator _rootIterator, std::string _childName)
{

    static const string FORMULATION_AND_ROUTE_SELECTION_OPTION_NODE          = "formulationAndRouteSelectionOption";

    Common::XmlNodeIterator optionsRootIterator = _rootIterator->getChildren(_childName);

    Common::XmlNodeIterator formulationAndRouteSelectionOptioneRootIterator = optionsRootIterator->getChildren(FORMULATION_AND_ROUTE_SELECTION_OPTION_NODE);


    static std::map<std::string,Tucuxi::Core::FormulationAndRouteSelectionOption> m =
    {
        {"allFormulationAndRoutes", Tucuxi::Core::FormulationAndRouteSelectionOption::AllFormulationAndRoutes},
        {"lastFormulationAndRoute", Tucuxi::Core::FormulationAndRouteSelectionOption::LastFormulationAndRoute},
        {"defaultFormulationAndRoute", Tucuxi::Core::FormulationAndRouteSelectionOption::DefaultFormulationAndRoute}
    };

    string value = formulationAndRouteSelectionOptioneRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(formulationAndRouteSelectionOptioneRootIterator);

    return Tucuxi::Core::FormulationAndRouteSelectionOption::LastFormulationAndRoute;
}


Tucuxi::Core::ComputingOption QueryImport::getChildComputingOption(Common::XmlNodeIterator _rootIterator, string _childName)
{
    static const string PARAMETERS_TYPE             = "parametersType";
    static const string COMPARTMENT_OPTION          = "compartmentOption";
    static const string RETRIEVE_STATISTICS         = "retrieveStatistics";
    static const string RETRIEVE_PARAMETERS         = "retrieveParameters";
    static const string RETRIEVE_COVARIATES         = "retrieveCovariates";

    Common::XmlNodeIterator computingOptionRootIterator = _rootIterator->getChildren(_childName);
    Tucuxi::Core::PredictionParameterType predictionParameterType = getChildParametersTypeEnum(computingOptionRootIterator, PARAMETERS_TYPE);
    Tucuxi::Core::CompartmentsOption compartmentOption = getChildCompartmentsOptionEnum(computingOptionRootIterator, COMPARTMENT_OPTION);
    Tucuxi::Core::RetrieveStatisticsOption retrieveStatisticsOption;
    Tucuxi::Core::RetrieveParametersOption retrieveParametersOption;
    Tucuxi::Core::RetrieveCovariatesOption retrieveCovariatesOption;

    if(getChildBoolValue(computingOptionRootIterator, RETRIEVE_STATISTICS))
    {
        retrieveStatisticsOption = Tucuxi::Core::RetrieveStatisticsOption::RetrieveStatistics;
    }else {
        retrieveStatisticsOption = Tucuxi::Core::RetrieveStatisticsOption::DoNotRetrieveStatistics;
    }

    if(getChildBoolValue(computingOptionRootIterator, RETRIEVE_PARAMETERS))
    {
        retrieveParametersOption = Tucuxi::Core::RetrieveParametersOption::RetrieveParameters;
    }else {
        retrieveParametersOption = Tucuxi::Core::RetrieveParametersOption::DoNotRetrieveParameters;
    }

    if(getChildBoolValue(computingOptionRootIterator, RETRIEVE_COVARIATES))
    {
        retrieveCovariatesOption = Tucuxi::Core::RetrieveCovariatesOption::RetrieveCovariates;
    }else {
        retrieveCovariatesOption = Tucuxi::Core::RetrieveCovariatesOption::DoNotRetrieveCovariates;
    }


    return Tucuxi::Core::ComputingOption(predictionParameterType, compartmentOption, retrieveStatisticsOption, retrieveParametersOption, retrieveCovariatesOption);
}



Tucuxi::Core::CompartmentsOption QueryImport::getChildCompartmentsOptionEnum(Common::XmlNodeIterator _rootIterator, std::string _childName)
{

    Common::XmlNodeIterator compartmentOptionRootIterator = _rootIterator->getChildren(_childName);

    static std::map<std::string,Tucuxi::Core::CompartmentsOption> m =
    {
        {"allActiveMoieties", Tucuxi::Core::CompartmentsOption::AllActiveMoieties},
        {"allAnalytes", Tucuxi::Core::CompartmentsOption::AllAnalytes},
        {"allCompartments", Tucuxi::Core::CompartmentsOption::AllCompartments},
        {"specific", Tucuxi::Core::CompartmentsOption::Specific}
    };

    string value = compartmentOptionRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(compartmentOptionRootIterator);

    return Tucuxi::Core::CompartmentsOption::AllActiveMoieties;
}

Tucuxi::Core::PredictionParameterType QueryImport::getChildParametersTypeEnum(Common::XmlNodeIterator _rootIterator, std::string _childName)
{


    Common::XmlNodeIterator parametersTypeRootIterator = _rootIterator->getChildren(_childName);

    static std::map<std::string,Tucuxi::Core::PredictionParameterType> m =
    {
        {"population", Tucuxi::Core::PredictionParameterType::Population},
        {"apriori", Tucuxi::Core::PredictionParameterType::Apriori},
        {"aposteriori", Tucuxi::Core::PredictionParameterType::Aposteriori}
//        {"best", Tucuxi::Core::CompartmentsOption::Specific}
    };

    string value = parametersTypeRootIterator->getValue();
    auto it = m.find(value);
    if (it != m.end()) {
        return it->second;
    }

    setNodeError(parametersTypeRootIterator);

    return Tucuxi::Core::PredictionParameterType::Population;
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
