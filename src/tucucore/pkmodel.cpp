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
#include "tucucore/pkmodels/rktwocompartmenterlang.h"

#ifdef DRUGMODELTESTS
#include "tucucore/../../test/tucucore/pkmodels/constanteliminationbolus.h"
#endif // DRUGMODELTESTS

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


bool PkModel::addIntakeIntervalCalculatorFactory(AbsorptionModel _route,
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
        return search->second->create();
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


/// \brief Add an Erlang absorption PkModel to a collection.
/// \param _COLLECTION Collection to which the PkModel has to be added.
/// \param _COMP_NO_NUM Number of transit compartments (expressed in numerical form).
/// \param _RC Boolean return type (ORed result of all the add operations).
#define ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_COLLECTION, _COMP_NO_NUM, _RC) \
do { \
    { \
        std::shared_ptr<PkModel> pkmodel = std::make_shared<PkModel>("linear.2comp.erlang" #_COMP_NO_NUM ".micro"); \
        rc |= pkmodel->addIntakeIntervalCalculatorFactory(AbsorptionModel::Extravascular, Tucuxi::Core::RK4TwoCompartmentErlangMicro<_COMP_NO_NUM>::getCreator()); \
        Tucuxi::Common::TranslatableString distribution; \
        Tucuxi::Common::TranslatableString elimination; \
        std::string comps; \
        comps = "compartments"; \
        distribution.setString(std::to_string(2) + " " + comps + "erlang absorption with " #_COMP_NO_NUM " transit compartments", "en"); \
        elimination.setString("linear", "en"); \
        pkmodel->setDistribution(distribution); \
        pkmodel->setElimination(elimination); \
        _collection.addPkModel(pkmodel); \
    } \
    { \
        std::shared_ptr<PkModel> pkmodel = std::make_shared<PkModel>("linear.2comp.erlang" #_COMP_NO_NUM ".macro"); \
        rc |= pkmodel->addIntakeIntervalCalculatorFactory(AbsorptionModel::Extravascular, Tucuxi::Core::RK4TwoCompartmentErlangMacro<_COMP_NO_NUM>::getCreator()); \
        Tucuxi::Common::TranslatableString distribution; \
        Tucuxi::Common::TranslatableString elimination; \
        std::string comps; \
        comps = "compartments"; \
        distribution.setString(std::to_string(2) + " " + comps + "erlang absorption with " #_COMP_NO_NUM " transit compartments", "en"); \
        elimination.setString("linear", "en"); \
        pkmodel->setDistribution(distribution); \
        pkmodel->setElimination(elimination); \
        _collection.addPkModel(pkmodel); \
    } \
} while (0);





bool defaultPopulate(PkModelCollection &_collection)
{
    bool rc = true;

    ADD_PKMODEL_TO_COLLECTION(_collection, 1, One, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 1, One, Micro, micro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 2, Two, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 2, Two, Micro, micro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 3, Three, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 3, Three, Micro, micro, rc);

    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 1, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 2, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 3, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 4, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 5, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 6, rc);

#ifdef DRUGMODELTESTS
    std::shared_ptr<PkModel> sharedPkModel;
    sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

    rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());

    _collection.addPkModel(sharedPkModel);

#endif // DRUGMODELTESTS

    return rc;
}

} // namespace Core
} // namespace Tucuxi
