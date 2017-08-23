/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "pkmodel.h"
#include "intakeintervalcalculator.h"
#include "onecompartmentbolus.h"

namespace Tucuxi {
namespace Core {

PkModel::PkModel(const std::string &_pkModelId)
    : m_pkModelId(_pkModelId)
{

}


std::string
PkModel::getPkModelId() const
{
    return m_pkModelId;
}


bool
PkModel::addIntakeIntervalCalculatorFactory(const RouteOfAdministration _route,
                                            std::shared_ptr<IntakeIntervalCalculatorCreator> _creator)
{
    std::pair<std::map<RouteOfAdministration, std::shared_ptr<IntakeIntervalCalculatorCreator>>::iterator, bool> rc;
    rc = m_calculatorCreators.insert(std::make_pair(_route, _creator));
    return rc.second;
}


std::vector<RouteOfAdministration>
PkModel::getAvailableRoutes() const
{
    std::vector<RouteOfAdministration> routes;
    // Push keys in a vector.
    std::transform(m_calculatorCreators.begin(), m_calculatorCreators.end(),                              // Range to inspect
                   std::back_inserter(routes),                                                            // Backward iterator on returned vector
                   [](decltype(m_calculatorCreators)::value_type const &_pair) { return _pair.first; });  // Push values in vector
    return routes;
}


std::shared_ptr<IntakeIntervalCalculator>
PkModel::getCalculatorForRoute(RouteOfAdministration _route) const
{
    auto search = m_calculatorCreators.find(_route);
    if(search != m_calculatorCreators.end()) {
        return search->second->create();
    }
    else {
        return nullptr;
    }
}


bool
PkModelCollection::addPkModel(std::shared_ptr<PkModel> _pkModel)
{
    // Check that no previous PkModel with the same Id was inserted in the collection.
    auto it = std::find_if(m_collection.begin(), m_collection.end(),
                           [_pkModel](const std::shared_ptr<PkModel> &_mod) { return _pkModel->getPkModelId() == _mod->getPkModelId(); });
    if (it != m_collection.end()) {
        return false;
    }
    m_collection.push_back(_pkModel);
    return true;
}


std::shared_ptr<PkModel>
PkModelCollection::getPkModelFromId(const std::string &_pkModelId) const
{
    auto it = std::find_if(m_collection.begin(), m_collection.end(),
                           [_pkModelId](const std::shared_ptr<PkModel> &_mod) { return _mod->getPkModelId() == _pkModelId; });
    if (it == m_collection.end()) {
        return nullptr;
    }
    return *it;
}


std::vector<std::string>
PkModelCollection::getPkModelIdList() const
{
    std::vector<std::string> ids;
    std::transform(m_collection.begin(), m_collection.end(),
                   std::back_inserter(ids),                                                     // Backward iterator on returned vector
                   [](const std::shared_ptr<PkModel> &_mod) { return _mod->getPkModelId(); });  // Push values in vector
    return ids;
}


std::vector<std::shared_ptr<PkModel>>
PkModelCollection::getPkModelList() const
{
    return m_collection;
}


/// \brief Add a PkModel to a collection.
/// \param _COLLECTION Collection to which the PkModel has to be added.
/// \param _COMP_NO_NUM Number of components (expressed in numerical form).
/// \param _COMP_NO_LIT Number of components (expressed in literal form).
/// \param _TYPE Type (either Micro or Macro).
/// \param _TYPE_NAME Type in litereal form (either micro or macro).
/// \param _RC Boolean return type (ORed result of all the add operations).
#define ADD_PKMODEL_TO_COLLECTION(_COLLECTION, _COMP_NO_NUM, _COMP_NO_LIT, _TYPE, _TYPE_NAME, _RC) \
do { \
    std::shared_ptr<PkModel> pkmodel = std::make_shared<PkModel>("linear." #_COMP_NO_NUM "comp." #_TYPE_NAME); \
    _RC |= pkmodel->addIntakeIntervalCalculatorFactory(RouteOfAdministration::EXTRAVASCULAR, Tucuxi::Core::_COMP_NO_LIT ## CompartmentExtra ## _TYPE::getCreator()); \
    _RC |= pkmodel->addIntakeIntervalCalculatorFactory(RouteOfAdministration::INTRAVASCULAR, Tucuxi::Core::_COMP_NO_LIT ## CompartmentBolus ## _TYPE::getCreator()); \
    _RC |= pkmodel->addIntakeIntervalCalculatorFactory(RouteOfAdministration::INFUSION, Tucuxi::Core::_COMP_NO_LIT ## CompartmentInfusion## _TYPE::getCreator());\
    _COLLECTION.addPkModel(pkmodel); \
} while (0);

bool
populate(PkModelCollection &_collection)
{
    bool rc = true;

    ADD_PKMODEL_TO_COLLECTION(_collection, 1, One, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 1, One, Micro, micro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 2, Two, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 2, Two, Micro, micro, rc);
//    ADD_PKMODEL_TO_COLLECTION(_collection, 3, Three, Macro, macro, rc);
//    ADD_PKMODEL_TO_COLLECTION(_collection, 3, Three, Micro, micro, rc);
    return rc;
}

}
}
