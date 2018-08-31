#include "formulationandroute.h"
#include "parameterdefinition.h"

#include "tucucommon/general.h"

namespace Tucuxi {
namespace Core {

const ParameterSetDefinition* FormulationAndRoute::getParameterDefinitions(const std::string &_analyteId) const
{
    for (const std::unique_ptr<AnalyteSetToAbsorptionAssociation>& association : m_associations) {
        for (const std::unique_ptr<Analyte>& analyte : association->m_analyteSet.getAnalytes()) {
            if (analyte->getAnalyteId() == _analyteId) {
                return association->m_absorptionParameters.get();
            }
        }
    }
    return nullptr;
}


void FormulationAndRoutes::add(std::unique_ptr<FormulationAndRoute> _far)
{
    m_fars.push_back(std::move(_far));
}


const FormulationAndRoute* FormulationAndRoutes::get(const Formulation& _formulation, Route _route) const
{
    TMP_UNUSED_PARAMETER(_formulation);

    for (const std::unique_ptr<FormulationAndRoute>& far : m_fars) {
        if (/*far->m_formulation == _formulation &&*/
            far->m_route == _route) {
            return far.get();
        }
    }
    return nullptr;
}

}
}
