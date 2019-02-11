/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef ANALYTE_H
#define ANALYTE_H

#include <string>

#include "tucucore/drugmodel/errormodel.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/drugmodel/parameterdefinition.h"

namespace Tucuxi {
namespace Core {

class ActiveSubstance;

class MolarMass
{
public:
    MolarMass(Value _value, Unit _unit) : m_value(_value), m_unit(_unit)
    {}

    MolarMass() {}

protected:

    Value m_value;
    Unit m_unit;
};

class Analyte
{
public:
    Analyte();

    Analyte(std::string _id, Unit _unit, MolarMass _molarMass) :
        m_analyteId(_id), m_unit(_unit), m_molarMass(_molarMass) {
    }

//    void setName(std::string _name) { m_name = _name;}
//    std::string getName() const { return m_name;}

    void setAnalyteId(std::string _analyteId) { m_analyteId = _analyteId;}
    std::string getAnalyteId() const { return m_analyteId;}

    void setResidualErrorModel(std::unique_ptr<ErrorModel> _residualErrorModel) { m_residualErrorModel = std::move(_residualErrorModel);}
    const ErrorModel& getResidualErrorModel() const { return *m_residualErrorModel; }

    const Unit &getUnit() { return m_unit;}

//    void addTarget(std::unique_ptr<TargetDefinition>& _target) { m_targets.push_back(std::move(_target));}
//    const TargetDefinitions & getTargetDefinitions() const { return m_targets;}

protected:

//    std::string m_name;
    std::string m_analyteId;

    Unit m_unit;

    std::unique_ptr<ErrorModel> m_residualErrorModel;

//    TargetDefinitions m_targets;

    ActiveSubstance *m_activeSubstance;

    MolarMass m_molarMass;

};

class ActiveSubstance : public Analyte
{
protected:

    std::string m_substanceId;

};


class AnalyteSet
{
public:
    const ParameterSetDefinition& getDispositionParameters() const { return *m_dispositionParameters; }
    void setDispositionParameters(std::unique_ptr<ParameterSetDefinition> _parameters) { m_dispositionParameters = std::move(_parameters);}

    void setId(std::string _id) { m_analyteSetId = _id;}
    std::string getId() const { return m_analyteSetId;}

    void addAnalyte(std::unique_ptr<Analyte> _analyte) { m_analytes.push_back(std::move(_analyte));}
    const std::vector<std::unique_ptr< Analyte > >& getAnalytes() const { return m_analytes;}

    void setPkModelId(std::string _pkModelId) { m_pkModelId = _pkModelId;}
    std::string getPkModelId() const { return m_pkModelId;}

protected:
    ///
    /// \brief m_analyteSetId
    /// If it contains only a single analyte, then should have the same Id as the analyte
    std::string m_analyteSetId;

    std::vector<std::unique_ptr< Analyte > > m_analytes;

    std::unique_ptr<ParameterSetDefinition> m_dispositionParameters;

    std::string m_pkModelId;
};

} // namespace Core
} // namespace Tucuxi


#endif // ANALYTE_H
