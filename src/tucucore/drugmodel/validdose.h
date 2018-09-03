/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef VALIDDOSE_H
#define VALIDDOSE_H

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/validvalues.h"

namespace Tucuxi {
namespace Core {



class ValidDoses : public ValidValues
{
public:
    ValidDoses(Unit _unit, std::unique_ptr<PopulationValue> _defaultDose);

    virtual ~ValidDoses();

    const std::vector<std::string>& getAnalyteIds() const;

    void setAnalyteIds(std::vector<std::string> _analyteIds);

protected:

    std::vector<std::string> m_analyteIds;
};



} // namespace Core
} // namespace Tucuxi


#endif // VALIDDOSE_H

