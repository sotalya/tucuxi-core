//@@license@@

#include "formulationandroute.h"

#include "tucucommon/general.h"

#include "parameterdefinition.h"

namespace Tucuxi {
namespace Core {

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

const std::vector<std::unique_ptr<FullFormulationAndRoute> >& FormulationAndRoutes::getList() const
{
    return m_fars;
}


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

} // namespace Core
} // namespace Tucuxi
