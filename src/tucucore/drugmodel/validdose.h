/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef VALIDDOSE_H
#define VALIDDOSE_H

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/invariants.h"
#include "tucucore/validvalues.h"


namespace Tucuxi {
namespace Core {


class ValidDoses : public ValidValues
{
public:
    ValidDoses(TucuUnit _unit, std::unique_ptr<PopulationValue> _defaultDose);

    ~ValidDoses() override;

    // No invariants
    EMPTYINVARIANTS
};



} // namespace Core
} // namespace Tucuxi


#endif // VALIDDOSE_H
