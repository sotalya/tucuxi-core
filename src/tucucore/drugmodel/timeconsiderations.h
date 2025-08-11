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


#ifndef TIMECONSIDERATIONS_H
#define TIMECONSIDERATIONS_H

#include <memory>

#include "tucucore/drugdefinitions.h"
#include "tucucore/invariants.h"


namespace Tucuxi {
namespace Core {

class HalfLife : public PopulationValue
{
public:
    HalfLife(
            const std::string& _id,
            Value _value,
            TucuUnit _unit,
            double _multiplier,
            std::unique_ptr<Operation> _operation = nullptr)
        : PopulationValue(_id, _value, std::move(_operation)), m_multiplier(_multiplier), m_unit(std::move(_unit))
    {
    }

    double getMultiplier() const
    {
        return m_multiplier;
    }
    TucuUnit getUnit() const
    {
        return m_unit;
    }

    INVARIANTS(INVARIANT(Invariants::INV_HALFLIFE_0001, (m_multiplier > 0.0), "A half life has a negative multiplier");
               INVARIANT(Invariants::INV_HALFLIFE_0002, (this->m_value > 0.0), "A half life has a negative value");)

private:
    double m_multiplier;

    TucuUnit m_unit;
};

class OutdatedMeasure : public PopulationValue
{
public:
    /// \brief Create a population value from its components.
    /// \param _id Identifier of the value.
    /// \param _value Default value.
    /// \param _operation Operation associated with the parameter.
    /// \param _unit Unit associated to the value.
    /// The operation has a default value of nullptr
    OutdatedMeasure(
            const std::string& _id, const Value _value, TucuUnit _unit, std::unique_ptr<Operation> _operation = nullptr)
        : PopulationValue(_id, _value, std::move(_operation)), m_unit(std::move(_unit))
    {
    }


    // No invariants
    EMPTYINVARIANTS

private:
    TucuUnit m_unit;
};

class TimeConsiderations
{
public:
    TimeConsiderations();

    void setHalfLife(std::unique_ptr<HalfLife> _halfLife)
    {
        m_halfLife = std::move(_halfLife);
    }

    const HalfLife& getHalfLife() const
    {
        return *m_halfLife;
    }

    void setOutdatedMeasure(std::unique_ptr<OutdatedMeasure> _outdatedMeasure)
    {
        m_outdatedMeasure = std::move(_outdatedMeasure);
    }

    const OutdatedMeasure& getOutdatedMeasure() const
    {
        return *m_outdatedMeasure;
    }

    INVARIANTS(INVARIANT(
                       Invariants::INV_TIMECONSIDERATIONS_0001,
                       (m_halfLife != nullptr),
                       "A time consideration has no half life");
               INVARIANT(
                       Invariants::INV_TIMECONSIDERATIONS_0002,
                       (m_halfLife->checkInvariants()),
                       "A half life has some errors");
               INVARIANT(
                       Invariants::INV_TIMECONSIDERATIONS_0003,
                       (m_outdatedMeasure != nullptr),
                       "A time consideration has no outdated measure field");
               INVARIANT(
                       Invariants::INV_TIMECONSIDERATIONS_0004,
                       (m_outdatedMeasure->checkInvariants()),
                       "An outdated measure field has some errors");)

private:
    std::unique_ptr<HalfLife> m_halfLife;
    std::unique_ptr<OutdatedMeasure> m_outdatedMeasure;
};

} // namespace Core
} // namespace Tucuxi

#endif // TIMECONSIDERATIONS_H
