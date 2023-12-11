//@@license@@

#ifndef ACTIVEMOIETY_H
#define ACTIVEMOIETY_H

#include "tucucommon/translatablestring.h"
#include "tucucommon/utils.h"

#include "tucucore/drugdefinitions.h"
#include "tucucore/drugmodel/analyte.h"
#include "tucucore/invariants.h"
#include "tucucore/operation.h"

#include "targetdefinition.h"

namespace Tucuxi {
namespace Core {

class ActiveMoiety
{
public:
    ActiveMoiety(
            ActiveMoietyId _id,
            TucuUnit _unit,
            std::vector<AnalyteId> _analyteIds,
            std::unique_ptr<Operation> _formula);
    ActiveMoiety();

    void addTarget(std::unique_ptr<TargetDefinition> _target)
    {
        m_targets.push_back(std::move(_target));
    }
    const TargetDefinitions& getTargetDefinitions() const
    {
        return m_targets;
    }

    ActiveMoietyId getActiveMoietyId() const
    {
        return m_id;
    }
    Tucuxi::Common::TranslatableString getActiveMoietyName() const
    {
        return m_name;
    }
    const std::vector<AnalyteId>& getAnalyteIds() const
    {
        return m_analyteIds;
    }

    Operation* getFormula() const
    {
        return m_formula.get();
    }

    void setName(const Tucuxi::Common::TranslatableString& _name)
    {
        m_name = _name;
    }

    TucuUnit getUnit() const
    {
        return m_unit;
    }

    INVARIANTS(std::string id = m_id.toString();
               INVARIANT(Invariants::INV_ACTIVEMOIETY_0001, (!m_id.empty()), "An active moiety has no Id");
               INVARIANT(
                       Invariants::INV_ACTIVEMOIETY_0002,
                       (!m_analyteIds.empty()),
                       Tucuxi::Common::Utils::strFormat(
                               "The active moiety %s has no analyte Ids", m_id.toString().c_str()));
               LAMBDA_INVARIANT(
                       Invariants::INV_ACTIVEMOIETY_0003,
                       {
                           bool ok = true;
                           for (const auto& analyteId : m_analyteIds) {
                               ok &= !analyteId.empty();
                           }
                           return ok;
                       },
                       Tucuxi::Common::Utils::strFormat(
                               "There is an empty analyte Id in the active moiety %s", id.c_str()));
               INVARIANT(
                       Invariants::INV_ACTIVEMOIETY_0004,
                       (m_formula != nullptr),
                       Tucuxi::Common::Utils::strFormat(
                               "The active moiety %s has no formula to calculate it", m_id.toString().c_str()));
               LAMBDA_INVARIANT(
                       Invariants::INV_ACTIVEMOIETY_0005,
                       {
                           bool ok = true;
                           for (const auto& target : m_targets) {
                               ok &= target->checkInvariants();
                           }
                           return ok;
                       },
                       Tucuxi::Common::Utils::strFormat(
                               "There is an error in a target of the active moiety %s", m_id.toString().c_str()));)

protected:
    ActiveMoietyId m_id;
    Tucuxi::Common::TranslatableString m_name;
    TucuUnit m_unit;
    std::vector<AnalyteId> m_analyteIds;
    std::unique_ptr<Operation> m_formula;

    TargetDefinitions m_targets;
};



typedef std::vector<std::unique_ptr<ActiveMoiety>> ActiveMoieties;

} // namespace Core
} // namespace Tucuxi

#endif // ACTIVEMOIETY_H
