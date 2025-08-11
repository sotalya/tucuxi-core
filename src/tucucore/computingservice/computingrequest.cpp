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


#include <utility>

#include "computingrequest.h"


namespace Tucuxi {
namespace Core {

ComputingRequest::ComputingRequest(
        RequestResponseId _id,
        const DrugModel& _drugModel,
        const DrugTreatment& _drugTreatment,
        std::unique_ptr<ComputingTrait> _computingTrait)
    : m_id(std::move(_id)), m_drugModel(_drugModel), m_drugTreatment(_drugTreatment)
{
    m_computingTraits = std::make_unique<ComputingTraits>();
    m_computingTraits->addTrait(std::move(_computingTrait));
}

RequestResponseId ComputingRequest::getId() const
{
    return m_id;
}

const DrugModel& ComputingRequest::getDrugModel() const
{
    return m_drugModel;
}

const DrugTreatment& ComputingRequest::getDrugTreatment() const
{
    return m_drugTreatment;
}

const ComputingTraits& ComputingRequest::getComputingTraits() const
{
    return *m_computingTraits;
}

} // namespace Core
} // namespace Tucuxi
