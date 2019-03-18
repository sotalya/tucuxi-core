/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef VALIDDOSE_H
#define VALIDDOSE_H

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/validvalues.h"
#include "tucucore/invariants.h"

namespace Tucuxi {
namespace Core {



class ValidDoses : public ValidValues
{
public:
    ValidDoses(Unit _unit, std::unique_ptr<PopulationValue> _defaultDose);

    virtual ~ValidDoses();

    const std::vector<std::string>& getAnalyteIds() const;

    // TODO : We should get rid of these analyte Ids
    void setAnalyteIds(std::vector<std::string> _analyteIds);

    // TODO : Add invariants
    INVARIANTS()

protected:

    std::vector<std::string> m_analyteIds;
};



} // namespace Core
} // namespace Tucuxi


#endif // VALIDDOSE_H

