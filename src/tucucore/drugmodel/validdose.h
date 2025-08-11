/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
