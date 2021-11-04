#ifndef TUCUXI_QUERY_PARAMETERSDATA_H
#define TUCUXI_QUERY_PARAMETERSDATA_H

#include <string>
#include <vector>
#include <memory>
#include "tucucommon/datetime.h"
#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/drugtreatment/target.h"
#include "tucucore/drugtreatment/sample.h"

namespace Tucuxi {
namespace Query {

class CovariateData
{
public:
    // Constructors
    CovariateData() = delete;

    CovariateData(
        std::string& _covariateId,
        Tucuxi::Common::DateTime& _pDate,
        std::string& _value,
        std::string& _unit,
        Tucuxi::Core::DataType _datatype,
        std::string& _nature
    );

    CovariateData(CovariateData& _other) = delete;

    // Getters
    const std::string& getCovariateId() const;
    const Tucuxi::Common::DateTime& getpDate() const;
    const std::string& getValue() const;
    const std::string& getUnit() const;
    Tucuxi::Core::DataType getDatatype() const;
    const std::string& getNature() const;

protected:
    const std::string m_covariateId;
    const Tucuxi::Common::DateTime m_pDate;
    const std::string m_value;
    const std::string m_unit;
    const Tucuxi::Core::DataType m_datatype;
    const std::string m_nature;
};

class PatientData
{
public:
    // Constructors
    PatientData() = delete;

    PatientData(std::vector< std::unique_ptr<Tucuxi::Core::PatientCovariate> >& _covariates);

    PatientData(PatientData& _other) = delete;

    // Getters
    const std::vector<std::unique_ptr<Tucuxi::Core::PatientCovariate> >& getCovariates() const;

protected:
    std::vector< std::unique_ptr<Tucuxi::Core::PatientCovariate> > m_covariates;
};

class Treatment
{
public:
    // Constructors
    Treatment() = delete;

    Treatment(std::unique_ptr<Tucuxi::Core::DosageHistory> _pDosageHistory);

    Treatment(Treatment& _other) = delete;

    // Getters
    const Tucuxi::Core::DosageHistory& getpDosageHistory() const;

protected:
    std::unique_ptr<Tucuxi::Core::DosageHistory> m_pDosageHistory;
};

//class ConcentrationData
//{
//public:
//    // Constructors
//    ConcentrationData() = delete;

//    ConcentrationData(
//        std::string _analyteID,
//        double _value,
//        std::string _unit
//    );

//    ConcentrationData(ConcentrationData& _other) = delete;

//    // Getters
//    std::string getAnalyteID() const;
//    double getValue() const;
//    std::string getUnit() const;

//protected:
//    const std::string m_analyteID;
//    const double m_value;
//    const std::string m_unit;
//};

//class SampleData
//{
//public:
//    // Constructors
//    SampleData() = delete;

//    SampleData(
//        std::string& _sampleID,
//        Tucuxi::Common::DateTime& _pSampleDate,
//        std::vector< std::unique_ptr<ConcentrationData> >& _concentrations
//    );

//    SampleData(SampleData& _other) = delete;

//    // Getters
//    std::string getSampleID() const;
//    const Tucuxi::Common::DateTime getpSampleDate() const;
//    const std::vector<std::unique_ptr<ConcentrationData> >& getConcentrations() const;

//protected:
//    const std::string m_sampleID;
//    const Tucuxi::Common::DateTime m_pSampleDate;
//    std::vector< std::unique_ptr<ConcentrationData> > m_concentrations;
//};

//class TargetData
//{
//public:
//    // Constructors
//    TargetData() = delete;

//    TargetData(
//        std::string& _activeMoietyID,
//        std::string& _targetType,
//        std::string& _unit,
//        double _inefficacyAlarm,
//        double _min,
//        double _best,
//        double _max,
//        double _toxicityAlarm,
//        std::string& _micUnit,
//        double _micValue
//    );

//    TargetData(TargetData& _other) = delete;

//    // Getters
//    std::string getActiveMoietyID() const;
//    Core::TargetType getTargetType() const;
//    std::string getUnit() const;
//    double getInefficacyAlarm() const;
//    double getMin() const;
//    double getBest() const;
//    double getMax() const;
//    double getToxicityAlarm() const;
//    std::string getMicUnit() const;
//    double getMicValue() const;


//protected:
//    const std::string m_activeMoietyID;
//    Core::TargetType m_targetType;
//    const std::string m_unit;
//    const double m_inefficacyAlarm;
//    const double m_min;
//    const double m_best;
//    const double m_max;
//    const double m_toxicityAlarm;
//    const std::string m_micUnit;
//    const double m_micValue;
//};

class DrugData
{
public:
    // Constructors
    DrugData() = delete;

    DrugData(std::string& _drugID,
        std::string& _activePrinciple,
        std::string& _brandName,
        std::string& _atc,
        std::unique_ptr<Treatment> _pTreatment,
        std::vector< std::unique_ptr<Tucuxi::Core::Sample> >& _samples,
        std::vector< std::unique_ptr<Tucuxi::Core::Target> >& _targets
    );

    DrugData(DrugData& _other) = delete;

    // Getters
    const std::string& getDrugID() const;
    const std::string& getActivePrinciple() const;
    const std::string& getBrandName() const;
    const std::string& getAtc() const;
    const Treatment& getpTreatment() const;
    const std::vector<std::unique_ptr<Tucuxi::Core::Sample> >& getSamples() const;
    const std::vector<std::unique_ptr<Tucuxi::Core::Target> >& getTargets() const;

protected:
    const std::string m_drugID;
    const std::string m_activePrinciple;
    const std::string m_brandName;
    const std::string m_atc;
    std::unique_ptr<Treatment> m_pTreatment;
    std::vector< std::unique_ptr<Tucuxi::Core::Sample> > m_samples;
    std::vector< std::unique_ptr<Tucuxi::Core::Target> > m_targets;
};

class DrugTreatmentData
{
public:
    // Constructors
    DrugTreatmentData() = delete;

    DrugTreatmentData(
        std::unique_ptr<PatientData> _pPatient,
        std::vector< std::unique_ptr<DrugData> > _drugs
    );

    DrugTreatmentData(DrugTreatmentData& _other) = delete;

    // Getters
    const PatientData& getpPatient() const;
    const std::vector<std::unique_ptr<DrugData> >& getDrugs() const;

protected:
    std::unique_ptr<PatientData> m_pPatient;
    std::vector< std::unique_ptr<DrugData> > m_drugs;

};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_PARAMETERSDATA_H
