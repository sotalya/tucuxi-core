#include "formulationandroute.h"
#include "parameterdefinition.h"

#include "tucucommon/general.h"

namespace Tucuxi {
namespace Core {

const ParameterSetDefinition* FullFormulationAndRoute::getParameterDefinitions(const AnalyteGroupId &_analyteGroupId) const
{
    for (const std::unique_ptr<AnalyteSetToAbsorptionAssociation>& association : m_associations) {
        if (association->m_analyteSet.getId() == _analyteGroupId) {
            return association->m_absorptionParameters.get();
        }
        /*
         * for (const std::unique_ptr<Analyte>& analyte : association->m_analyteSet.getAnalytes()) {
            if (analyte->getAnalyteId() == _analyteGroupId) {
                return association->m_absorptionParameters.get();
            }
        }*/
    }
    return nullptr;
}


FormulationAndRoutes::FormulationAndRoutes() :
    m_defaultIndex(0)
{

}


void FormulationAndRoutes::add(std::unique_ptr<FullFormulationAndRoute> _far, bool _isDefault)
{
    m_fars.push_back(std::move(_far));
    if (_isDefault) {
        m_defaultIndex = m_fars.size() - 1;
    }
}

const FullFormulationAndRoute* FormulationAndRoutes::getDefault() const
{
    if (m_fars.size() == 0) {
        return nullptr;
    }
    return m_fars.at(m_defaultIndex).get();
}

const FullFormulationAndRoute* FormulationAndRoutes::get(const Formulation& _formulation, AdministrationRoute _route) const
{
    TMP_UNUSED_PARAMETER(_formulation);

    for (const std::unique_ptr<FullFormulationAndRoute>& far : m_fars) {
        if (
            far->m_specs.getAdministrationRoute() == _route) {
            return far.get();
        }
    }
    return nullptr;
}


const FullFormulationAndRoute* FormulationAndRoutes::get(const FormulationAndRoute& _formulation) const
{
    for (const std::unique_ptr<FullFormulationAndRoute>& far : m_fars) {
        if (far->m_specs == _formulation) {
            return far.get();
        }
    }
    return nullptr;
}

const std::vector<std::unique_ptr<FullFormulationAndRoute> >& FormulationAndRoutes::getList() const
{
    return m_fars;
}

}
}
