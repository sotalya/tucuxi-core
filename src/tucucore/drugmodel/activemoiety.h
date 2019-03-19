#ifndef ACTIVEMOIETY_H
#define ACTIVEMOIETY_H

#include "tucucommon/translatablestring.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/operation.h"
#include "tucucore/invariants.h"
#include "targetdefinition.h"

namespace Tucuxi {
namespace Core {

class ActiveMoiety
{
public:
    ActiveMoiety(std::string _id, Unit _unit, std::vector<std::string> _analyteIds,std::unique_ptr<Operation> _formula);
    ActiveMoiety();

    void addTarget(std::unique_ptr<TargetDefinition> _target) { m_targets.push_back(std::move(_target));}
    const TargetDefinitions & getTargetDefinitions() const { return m_targets;}

    std::string getActiveMoietyId() const { return m_id;}
   Tucuxi::Common:: TranslatableString getActiveMoietyName() const { return m_name;}
    const std::vector<std::string> & getAnalyteIds() const {return m_analyteIds;}

    void setName(const Tucuxi::Common::TranslatableString &_name) { m_name = _name;}

    INVARIANTS(
            INVARIANT(Invariants::INV_ACTIVEMOIETY_0001, (m_id.size() > 0))
            INVARIANT(Invariants::INV_ACTIVEMOIETY_0002, (m_analyteIds.size() > 0))
            INVARIANT(Invariants::INV_ACTIVEMOIETY_0003, (m_formula != nullptr))
            LAMBDA_INVARIANT(Invariants::INV_ACTIVEMOIETY_0004, {bool ok = true;for(size_t i = 0; i < m_targets.size(); i++) {ok &= m_targets.at(i)->checkInvariants();} return ok;})
            )

protected:
    std::string m_id;
    Tucuxi::Common::TranslatableString m_name;
    Unit m_unit;
    std::vector<std::string> m_analyteIds;
    std::unique_ptr<Operation> m_formula;

    TargetDefinitions m_targets;

};



typedef std::vector<std::unique_ptr<ActiveMoiety>> ActiveMoieties;

} // namespace Core
} // namespace Tucuxi

#endif // ACTIVEMOIETY_H
