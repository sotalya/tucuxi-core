/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "pkmodel.h"
#include "intakeintervalcalculator.h"
#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"

namespace Tucuxi {
namespace Core {

PkModel::PkModel(const std::string &_pkModelId)
    : m_pkModelId(_pkModelId)
{
}


std::string PkModel::getPkModelId() const
{
    return m_pkModelId;
}


bool PkModel::addIntakeIntervalCalculatorFactory(const AbsorptionModel _route,
                                                 std::shared_ptr<IntakeIntervalCalculatorCreator> _creator)
{
    std::pair<std::map<AbsorptionModel, std::shared_ptr<IntakeIntervalCalculatorCreator>>::iterator, bool> rc;
    rc = m_calculatorCreators.insert(std::make_pair(_route, _creator));
    return rc.second;
}


std::vector<AbsorptionModel> PkModel::getAvailableRoutes() const
{
    std::vector<AbsorptionModel> routes;
    // Push keys in a vector.
    std::transform(m_calculatorCreators.begin(), m_calculatorCreators.end(),                              // Range to inspect
                   std::back_inserter(routes),                                                            // Backward iterator on returned vector
                   [](decltype(m_calculatorCreators)::value_type const &_pair) { return _pair.first; });  // Push values in vector
    return routes;
}


std::shared_ptr<IntakeIntervalCalculator> PkModel::getCalculatorForRoute(AbsorptionModel _route) const
{
    auto search = m_calculatorCreators.find(_route);
    if (search != m_calculatorCreators.end()) {
        return std::move(search->second->create());
    }
    else {
        return nullptr;
    }
}


bool PkModelCollection::addPkModel(std::shared_ptr<PkModel> _pkModel)
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


std::shared_ptr<PkModel> PkModelCollection::getPkModelFromId(const std::string &_pkModelId) const
{
    auto it = std::find_if(m_collection.begin(), m_collection.end(),
                           [_pkModelId](const std::shared_ptr<PkModel> &_mod) { return _mod->getPkModelId() == _pkModelId; });
    if (it == m_collection.end()) {
        return nullptr;
    }
    return *it;
}


std::vector<std::string> PkModelCollection::getPkModelIdList() const
{
    std::vector<std::string> ids;
    std::transform(m_collection.begin(), m_collection.end(),
                   std::back_inserter(ids),                                                     // Backward iterator on returned vector
                   [](const std::shared_ptr<PkModel> &_mod) { return _mod->getPkModelId(); });  // Push values in vector
    return ids;
}


std::vector<std::shared_ptr<PkModel>> PkModelCollection::getPkModelList() const
{
    return m_collection;
}


bool defaultPopulate(PkModelCollection &_collection)
{
    bool rc = true;

    ADD_PKMODEL_TO_COLLECTION(_collection, 1, One, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 1, One, Micro, micro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 2, Two, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 2, Two, Micro, micro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 3, Three, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 3, Three, Micro, micro, rc);
    return rc;
}

}
}
