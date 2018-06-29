/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef ANALYTE_H
#define ANALYTE_H

#include <string>

#include "tucucore/residualerrormodel.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/drugmodel/parameterdefinition.h"

namespace Tucuxi {
namespace Core {

class ActiveSubstance;

class Analyte
{
public:
    Analyte();

//    void setName(std::string _name) { m_name = _name;}
//    std::string getName() const { return m_name;}

    void setAnalyteId(std::string _analyteId) { m_analyteId = _analyteId;}
    std::string getAnalyteId() const { return m_analyteId;}

    void setResidualErrorModel(std::unique_ptr<IResidualErrorModel>& _residualErrorModel) { m_residualErrorModel = std::move(_residualErrorModel);}
    const IResidualErrorModel& getResidualErrorModel() const { return *m_residualErrorModel; }

//    void addTarget(std::unique_ptr<TargetDefinition>& _target) { m_targets.push_back(std::move(_target));}
//    const TargetDefinitions & getTargetDefinitions() const { return m_targets;}

protected:

//    std::string m_name;
    std::string m_analyteId;

    std::unique_ptr<IResidualErrorModel> m_residualErrorModel;

//    TargetDefinitions m_targets;

    ActiveSubstance *m_activeSubstance;

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

}
}


#endif // ANALYTE_H
