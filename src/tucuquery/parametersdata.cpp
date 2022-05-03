//@@license@@

#include "querydata.h"

using namespace std;
using namespace Tucuxi;

namespace Tucuxi {
namespace Query {

// CovariateData class

CovariateData::CovariateData(
        string& _covariateId,
        Common::DateTime& _pDate,
        string& _value,
        string& _unit,
        Core::DataType _datatype,
        string& _nature)
    : m_covariateId(_covariateId), m_pDate(_pDate), m_value(_value), m_unit(_unit), m_datatype(_datatype),
      m_nature(_nature)
{
}

const string& CovariateData::getCovariateId() const
{
    return m_covariateId;
}

const Common::DateTime& CovariateData::getpDate() const
{
    return m_pDate;
}

const string& CovariateData::getValue() const
{
    return m_value;
}

const string& CovariateData::getUnit() const
{
    return m_unit;
}

Core::DataType CovariateData::getDatatype() const
{
    return m_datatype;
}

const std::string& CovariateData::getNature() const
{
    return m_nature;
}

// PatientData class

PatientData::PatientData(vector<unique_ptr<Tucuxi::Core::PatientCovariate> >& _covariates)
    : m_covariates(move(_covariates))
{
}

const vector<unique_ptr<Tucuxi::Core::PatientCovariate> >& PatientData::getCovariates() const
{
    return m_covariates;
}

// Treatment class

Treatment::Treatment(unique_ptr<Core::DosageHistory> _pDosageHistory) : m_pDosageHistory(move(_pDosageHistory)) {}

const Core::DosageHistory& Treatment::getpDosageHistory() const
{
    return *m_pDosageHistory;
}

// ConcentrationData class

//ConcentrationData::ConcentrationData(string _analyteID, double _value, string _unit)
//    : m_analyteID(_analyteID), m_value(_value), m_unit(_unit)
//{}

//string ConcentrationData::getAnalyteID() const
//{
//    return m_analyteID;
//}

//double ConcentrationData::getValue() const
//{
//    return m_value;
//}

//string ConcentrationData::getUnit() const
//{
//    return m_unit;
//}

//// SampleData class

//SampleData::SampleData(
//        string& _sampleID,
//        Common::DateTime& _pSampleDate,
//        vector<unique_ptr<ConcentrationData> >& _concentrations
//) : m_sampleID(_sampleID), m_pSampleDate(_pSampleDate),
//    m_concentrations(move(_concentrations))
//{}

//string SampleData::getSampleID() const
//{
//    return m_sampleID;
//}

//const Common::DateTime SampleData::getpSampleDate() const
//{
//    return m_pSampleDate;
//}

//const vector<unique_ptr<ConcentrationData> >& SampleData::getConcentrations() const
//{
//    return m_concentrations;
//}


//// TargetData class

//TargetData::TargetData(
//        string& _activeMoietyID,
//        string& _targetType,
//        string& _unit,
//        double _inefficacyAlarm,
//        double _min,
//        double _best,
//        double _max,
//        double _toxicityAlarm,
//        string& _micUnit,
//        double _micValue
//) : m_activeMoietyID(_activeMoietyID), m_unit(_unit),
//    m_inefficacyAlarm(_inefficacyAlarm), m_min(_min), m_best(_best),
//    m_max(_max), m_toxicityAlarm(_toxicityAlarm), m_micUnit(_micUnit),
//    m_micValue(_micValue)
//{
//    if (_targetType == "peak") {
//        m_targetType = Core::TargetType::Peak;
//    } else if (_targetType == "residual") {
//        m_targetType = Core::TargetType::Residual;
//    } else if (_targetType == "mean") {
//        m_targetType = Core::TargetType::Mean;
//    } else if (_targetType == "auc") {
//        m_targetType = Core::TargetType::Auc;
//    } else if (_targetType == "aucOverMic") {
//        m_targetType = Core::TargetType::AucOverMic;
//    } else if (_targetType == "timeOverMic") {
//        m_targetType = Core::TargetType::TimeOverMic;
//    } else if (_targetType == "aucDividedByMic") {
//        m_targetType = Core::TargetType::AucDividedByMic;
//    } else if (_targetType == "peakDividedByMic") {
//        m_targetType = Core::TargetType::PeakDividedByMic;
//    } else {
//        m_targetType = Core::TargetType::UnknownTarget;
//    }
//}

//string TargetData::getActiveMoietyID() const
//{
//    return m_activeMoietyID;
//}

//Core::TargetType TargetData::getTargetType() const
//{
//    return m_targetType;
//}

//string TargetData::getUnit() const
//{
//    return m_unit;
//}

//double TargetData::getInefficacyAlarm() const
//{
//    return m_inefficacyAlarm;
//}

//double TargetData::getMin() const
//{
//    return m_min;
//}

//double TargetData::getBest() const
//{
//    return m_best;
//}

//double TargetData::getMax() const
//{
//    return m_max;
//}

//double TargetData::getToxicityAlarm() const
//{
//    return m_toxicityAlarm;
//}

//string TargetData::getMicUnit() const
//{
//    return m_micUnit;
//}

//double TargetData::getMicValue() const
//{
//    return m_micValue;
//}

// DrugData class

DrugData::DrugData(
        string& _drugID,
        string& _activePrinciple,
        string& _brandName,
        string& _atc,
        unique_ptr<Treatment> _pTreatment,
        vector<unique_ptr<Tucuxi::Core::Sample> >& _samples,
        vector<unique_ptr<Tucuxi::Core::Target> >& _targets)
    : m_drugID(_drugID), m_activePrinciple(_activePrinciple), m_brandName(_brandName), m_atc(_atc),
      m_pTreatment(move(_pTreatment)), m_samples(move(_samples)), m_targets(move(_targets))
{
}

const string& DrugData::getDrugID() const
{
    return m_drugID;
}

const string& DrugData::getActivePrinciple() const
{
    return m_activePrinciple;
}

const string& DrugData::getBrandName() const
{
    return m_brandName;
}

const string& DrugData::getAtc() const
{
    return m_atc;
}

const Treatment& DrugData::getpTreatment() const
{
    return *m_pTreatment;
}

const vector<unique_ptr<Tucuxi::Core::Sample> >& DrugData::getSamples() const
{
    return m_samples;
}

const vector<unique_ptr<Tucuxi::Core::Target> >& DrugData::getTargets() const
{
    return m_targets;
}

// ParametersData class

DrugTreatmentData::DrugTreatmentData(unique_ptr<PatientData> _pPatient, vector<unique_ptr<DrugData> > _drugs)
    : m_pPatient(move(_pPatient)), m_drugs(move(_drugs))
{
}

const PatientData& DrugTreatmentData::getpPatient() const
{
    return *m_pPatient;
}

const vector<unique_ptr<DrugData> >& DrugTreatmentData::getDrugs() const
{
    return m_drugs;
}

} // namespace Query
} // namespace Tucuxi
