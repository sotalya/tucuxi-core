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


#include "formulationandroute.h"

#include "tucucommon/general.h"

#include "parameterdefinition.h"

namespace Tucuxi {
namespace Core {

std::vector<FormulationAndRoute> mergeFormulationAndRouteList(
        const std::vector<FormulationAndRoute>& _v1, const std::vector<FormulationAndRoute>& _v2)
{
    std::vector<FormulationAndRoute> result;
    result = _v1;

    for (const auto& dd : _v2) {
        if (std::find_if(
                    result.begin(),
                    result.end(),
                    [&dd](const FormulationAndRoute& _f) -> bool { return _f.isCompatible(dd); })
            == result.end()) {
            result.push_back(dd);
        }
    }

    return result;
}


const ParameterSetDefinition* FullFormulationAndRoute::getParameterDefinitions(
        const AnalyteGroupId& _analyteGroupId) const
{
    for (const std::unique_ptr<AnalyteSetToAbsorptionAssociation>& association : m_associations) {
        if (association->m_analyteSet.getId() == _analyteGroupId) {
            return association->m_absorptionParameters.get();
        }
    }
    return nullptr;
}


AbsorptionModel FullFormulationAndRoute::getAbsorptionModel(const AnalyteGroupId& _analyteGroupId) const
{
    for (const std::unique_ptr<AnalyteSetToAbsorptionAssociation>& association : m_associations) {
        if (association->m_analyteSet.getId() == _analyteGroupId) {
            return association->m_absorptionModel;
        }
    }
    return AbsorptionModel::Undefined;
}

FormulationAndRoutes::FormulationAndRoutes() : m_defaultIndex(0) {}


void FormulationAndRoutes::add(std::unique_ptr<FullFormulationAndRoute> _far, bool _isDefault)
{
    m_fars.push_back(std::move(_far));
    if (_isDefault) {
        m_defaultIndex = m_fars.size() - 1;
    }
}

const FullFormulationAndRoute* FormulationAndRoutes::getDefault() const
{
    if (m_fars.empty()) {
        return nullptr;
    }
    return m_fars[m_defaultIndex].get();
}

const FullFormulationAndRoute* FormulationAndRoutes::get(
        const Formulation& _formulation, AdministrationRoute _route) const
{
    TMP_UNUSED_PARAMETER(_formulation);

    for (const std::unique_ptr<FullFormulationAndRoute>& far : m_fars) {
        if (far->m_specs.getAdministrationRoute() == _route) {
            return far.get();
        }
    }
    return nullptr;
}


const FullFormulationAndRoute* FormulationAndRoutes::get(const FormulationAndRoute& _formulation) const
{
    for (const std::unique_ptr<FullFormulationAndRoute>& far : m_fars) {
        if (far->m_specs.isCompatible(_formulation)) {
            return far.get();
        }
    }
    return nullptr;
}

const FullFormulationAndRoute* FormulationAndRoutes::get(const DMFormulationAndRoute& _formulation) const
{
    for (const std::unique_ptr<FullFormulationAndRoute>& far : m_fars) {
        if (far->m_specs.isCompatible(_formulation)) {
            return far.get();
        }
    }
    return nullptr;
}

const std::vector<std::unique_ptr<FullFormulationAndRoute> >& FormulationAndRoutes::getList() const
{
    return m_fars;
}


std::vector<DMFormulationAndRoute> mergeFormulationAndRouteList(
        const std::vector<DMFormulationAndRoute>& _v1, const std::vector<DMFormulationAndRoute>& _v2)
{
    std::vector<DMFormulationAndRoute> result;
    result = _v1;

    for (const auto& dd : _v2) {
        if (std::find_if(
                    result.begin(),
                    result.end(),
                    [&dd](const DMFormulationAndRoute& _f) -> bool { return _f.isCompatible(dd); })
            == result.end()) {
            result.push_back(dd);
        }
    }

    return result;
}

} // namespace Core
} // namespace Tucuxi
