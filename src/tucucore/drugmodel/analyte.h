/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef ANALYTE_H
#define ANALYTE_H

#include <string>

#include "tucucommon/utils.h"

#include "tucucore/drugmodel/errormodel.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/drugmodel/targetdefinition.h"
#include "tucucore/invariants.h"

namespace Tucuxi {
namespace Core {

class ActiveSubstance;

class MolarMass
{
public:
    MolarMass(Value _value, TucuUnit _unit) : m_value(_value), m_unit(std::move(_unit)) {}

    MolarMass() {}

protected:
    Value m_value{0.0};
    TucuUnit m_unit;
};

///
/// \brief The AnalyteId class
/// This class is simply a std::string.
/// The rationale is that it makes it mandatory to use AnalyteId wherever
/// needed, and as such not to mix things with AnalyteGroupId.
/// Therefore, the code is safer, and more readable.
///
class AnalyteId
{
public:
    explicit AnalyteId(std::string _s) : m_s(std::move(_s)) {}
    explicit AnalyteId(const char* _s) : m_s(_s) {}
    size_t size() const
    {
        return m_s.size();
    }
    bool empty() const
    {
        return m_s.empty();
    }

    std::string toString() const
    {
        return m_s;
    }

    inline bool operator==(const AnalyteId& _other)
    {
        return this->m_s == _other.m_s;
    }
    inline bool operator<(const AnalyteId& _other) const
    {
        return this->m_s < _other.m_s;
    }

protected:
    std::string m_s;
};

inline bool operator==(const AnalyteId& _lhs, const AnalyteId& _rhs)
{
    return _lhs.toString() == _rhs.toString();
}

inline bool operator!=(const AnalyteId& _lhs, const AnalyteId& _rhs)
{
    return _lhs.toString() != _rhs.toString();
}

///
/// \brief The AnalyteGroupId class
/// This class is simply a std::string.
/// The rationale is that it makes it mandatory to use AnalyteGroupId wherever
/// needed, and as such not to mix things with AnalyteId.
/// Therefore, the code is safer, and more readable.
///
class AnalyteGroupId
{
public:
    AnalyteGroupId(std::string _s) : m_s(std::move(_s)) {}
    AnalyteGroupId(const char* _s) : m_s(_s) {}

    size_t size() const
    {
        return m_s.size();
    }
    bool empty() const
    {
        return m_s.empty();
    }

    std::string toString() const
    {
        return m_s;
    }

    inline bool operator==(const AnalyteGroupId& _other)
    {
        return this->m_s == _other.m_s;
    }
    inline bool operator<(const AnalyteGroupId& _other)
    {
        return this->m_s < _other.m_s;
    }


protected:
    std::string m_s;
};

inline bool operator==(const AnalyteGroupId& _lhs, const AnalyteGroupId& _rhs)
{
    return _lhs.toString() == _rhs.toString();
}

inline bool operator<(const AnalyteGroupId& _lhs, const AnalyteGroupId& _rhs)
{
    return _lhs.toString() < _rhs.toString();
}


class Analyte
{
public:
    Analyte();

    Analyte(std::string _id, TucuUnit _unit, std::unique_ptr<MolarMass> _molarMass)
        : m_analyteId(std::move(_id)), m_unit(std::move(_unit)), m_molarMass(std::move(_molarMass))
    {
    }

    //    void setName(std::string _name) { m_name = _name;}
    //    std::string getName() const { return m_name;}

    void setAnalyteId(std::string _analyteId)
    {
        m_analyteId = AnalyteId(std::move(_analyteId));
    }
    AnalyteId getAnalyteId() const
    {
        return m_analyteId;
    }

    void setResidualErrorModel(std::unique_ptr<ErrorModel> _residualErrorModel)
    {
        m_residualErrorModel = std::move(_residualErrorModel);
    }
    const ErrorModel& getResidualErrorModel() const
    {
        return *m_residualErrorModel;
    }

    const TucuUnit& getUnit()
    {
        return m_unit;
    }

    INVARIANTS(INVARIANT(Invariants::INV_ANALYTE_0001, (!m_analyteId.empty()), "An analyte does not have an Id");
               INVARIANT(
                       Invariants::INV_ANALYTE_0002,
                       (m_residualErrorModel != nullptr),
                       Tucuxi::Common::Utils::strFormat(
                               "The analyte %s does not have a residual error model", m_analyteId.toString().c_str()));
               INVARIANT(
                       Invariants::INV_ANALYTE_0003,
                       (m_residualErrorModel->checkInvariants()),
                       Tucuxi::Common::Utils::strFormat(
                               "The analyte %s has an error in the error model", m_analyteId.toString().c_str()));
               INVARIANT(
                       Invariants::INV_ANALYTE_0004,
                       (!m_unit.isEmpty()),
                       Tucuxi::Common::Utils::strFormat("The analyte %s has no unit", m_analyteId.toString().c_str()));)

protected:
    AnalyteId m_analyteId;

    TucuUnit m_unit;

    std::unique_ptr<ErrorModel> m_residualErrorModel;

    std::unique_ptr<MolarMass> m_molarMass;
};

class ActiveSubstance : public Analyte
{
protected:
    std::string m_substanceId;
};


class AnalyteSet
{
public:
    AnalyteSet() : m_analyteSetId("") {}

    const ParameterSetDefinition& getDispositionParameters() const
    {
        return *m_dispositionParameters;
    }
    void setDispositionParameters(std::unique_ptr<ParameterSetDefinition> _parameters)
    {
        m_dispositionParameters = std::move(_parameters);
    }

    void setId(AnalyteGroupId _id)
    {
        m_analyteSetId = std::move(_id);
    }
    AnalyteGroupId getId() const
    {
        return m_analyteSetId;
    }

    void addAnalyte(std::unique_ptr<Analyte> _analyte)
    {
        m_analytes.push_back(std::move(_analyte));
    }
    const std::vector<std::unique_ptr<Analyte> >& getAnalytes() const
    {
        return m_analytes;
    }

    void setPkModelId(std::string _pkModelId)
    {
        m_pkModelId = std::move(_pkModelId);
    }
    std::string getPkModelId() const
    {
        return m_pkModelId;
    }

    void setDoseUnit(TucuUnit _unit)
    {
        m_doseUnit = std::move(_unit);
    }
    TucuUnit getDoseUnit() const
    {
        return m_doseUnit;
    }

    TucuUnit getConcentrationUnit() const
    {
        return m_analytes[0]->getUnit();
    }

    INVARIANTS(
            INVARIANT(Invariants::INV_ANALYTESET_0001, (!m_analyteSetId.empty()), "An analyte group has no Id");
            INVARIANT(
                    Invariants::INV_ANALYTESET_0002,
                    (!m_pkModelId.empty()),
                    Tucuxi::Common::Utils::strFormat(
                            "The analyte group %s has no PK model Id", m_analyteSetId.toString().c_str()));
            INVARIANT(
                    Invariants::INV_ANALYTESET_0003,
                    (!m_analytes.empty()),
                    Tucuxi::Common::Utils::strFormat(
                            "The analyte group %s has no analytes", m_analyteSetId.toString().c_str()));
            LAMBDA_INVARIANT(
                    Invariants::INV_ANALYTESET_0004,
                    {
                        bool ok = true;
                        for (size_t i = 0; i < m_analytes.size(); i++) {
                            ok &= m_analytes[i]->checkInvariants();
                        }
                        return ok;
                    },
                    Tucuxi::Common::Utils::strFormat(
                            "There is an error in an analyte of the analyte group %s",
                            m_analyteSetId.toString().c_str()));
            INVARIANT(
                    Invariants::INV_ANALYTESET_0005,
                    (m_dispositionParameters != nullptr),
                    Tucuxi::Common::Utils::strFormat(
                            "The analyte group %s has no disposition parameters", m_analyteSetId.toString().c_str()));
            INVARIANT(
                    Invariants::INV_ANALYTESET_0006,
                    (m_dispositionParameters->getNbParameters() > 0),
                    Tucuxi::Common::Utils::strFormat(
                            "The analyte group %s has no disposition parameters", m_analyteSetId.toString().c_str()));
            INVARIANT(
                    Invariants::INV_ANALYTESET_0007,
                    (m_dispositionParameters->checkInvariants()),
                    Tucuxi::Common::Utils::strFormat(
                            "The analyte group %s has an error in its disposition parameters",
                            m_analyteSetId.toString().c_str()));)

protected:
    ///
    /// \brief m_analyteSetId
    /// If it contains only a single analyte, then should have the same Id as the analyte
    AnalyteGroupId m_analyteSetId;

    std::vector<std::unique_ptr<Analyte> > m_analytes;

    std::unique_ptr<ParameterSetDefinition> m_dispositionParameters;

    std::string m_pkModelId;

    TucuUnit m_doseUnit{"ug"};
};

} // namespace Core
} // namespace Tucuxi


#endif // ANALYTE_H
