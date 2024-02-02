//@@license@@

#include <utility>

#include "pkmodel.h"

#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/onecompartmentextralag.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/rkmichaelismentenenzyme.h"
#include "tucucore/pkmodels/rkmichaelismentenonecomp.h"
#include "tucucore/pkmodels/rkmichaelismentenonecompvmaxamount.h"
#include "tucucore/pkmodels/rkmichaelismententwocomp.h"
#include "tucucore/pkmodels/rkmichaelismententwocompvmaxamount.h"
#include "tucucore/pkmodels/rktwocompartmenterlang.h"
#include "tucucore/pkmodels/rktwocompartmentextralag.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentextralag.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"

#include "intakeintervalcalculator.h"

#ifdef DRUGMODELTESTS
#include "tucucore/../../test/tucucore/pkmodels/constanteliminationbolus.h"
#include "tucucore/../../test/tucucore/pkmodels/multiconstanteliminationbolus.h"
#include "tucucore/../../test/tucucore/pkmodels/pkasymptotic.h"
#endif // DRUGMODELTESTS

namespace Tucuxi {
namespace Core {

PkModel::PkModel(std::string _pkModelId, AllowMultipleRoutes _allowMultipleRoutes)
    : m_pkModelId(std::move(_pkModelId)), m_allowMultipleRoutes(_allowMultipleRoutes)
{
}


std::string PkModel::getPkModelId() const
{
    return m_pkModelId;
}


bool PkModel::addIntakeIntervalCalculatorFactory(
        AbsorptionModel _route, const std::shared_ptr<IntakeIntervalCalculatorCreator>& _creator)
{
    std::pair<std::map<AbsorptionModel, std::shared_ptr<IntakeIntervalCalculatorCreator>>::iterator, bool> rc;
    rc = m_calculatorCreators.insert(std::make_pair(_route, _creator));
    return rc.second;
}


bool PkModel::addParameterList(AbsorptionModel _route, std::vector<std::string> _parameterList)
{
    std::pair<std::map<AbsorptionModel, std::vector<std::string>>::iterator, bool> rc;
    rc = m_parameters.insert(std::make_pair(_route, std::move(_parameterList)));
    return rc.second;
}

std::vector<AbsorptionModel> PkModel::getAvailableRoutes() const
{
    std::vector<AbsorptionModel> routes;
    // Push keys in a vector.
    std::transform(
            m_calculatorCreators.begin(),
            m_calculatorCreators.end(), // Range to inspect
            std::back_inserter(routes), // Backward iterator on returned vector
            [](decltype(m_calculatorCreators)::value_type const& _pair) {
                return _pair.first;
            }); // Push values in vector
    return routes;
}


std::shared_ptr<IntakeIntervalCalculator> PkModel::getCalculatorForRoute(AbsorptionModel _route) const
{
    auto search = m_calculatorCreators.find(_route);
    if (search != m_calculatorCreators.end()) {
        return search->second->create();
    }
    return nullptr;
}


std::vector<std::string> PkModel::getParametersForRoute(AbsorptionModel _route) const
{

    auto search = m_parameters.find(_route);
    if (search != m_parameters.end()) {
        return search->second;
    }
    return std::vector<std::string>();
}


bool PkModelCollection::addPkModel(const std::shared_ptr<PkModel>& _pkModel)
{
    // Check that no previous PkModel with the same Id was inserted in the collection.
    auto it = std::find_if(m_collection.begin(), m_collection.end(), [_pkModel](const std::shared_ptr<PkModel>& _mod) {
        return _pkModel->getPkModelId() == _mod->getPkModelId();
    });
    if (it != m_collection.end()) {
        return false;
    }
    m_collection.push_back(_pkModel);
    return true;
}


std::shared_ptr<PkModel> PkModelCollection::getPkModelFromId(const std::string& _pkModelId) const
{
    auto it =
            std::find_if(m_collection.begin(), m_collection.end(), [_pkModelId](const std::shared_ptr<PkModel>& _mod) {
                return _mod->getPkModelId() == _pkModelId;
            });
    if (it == m_collection.end()) {
        return nullptr;
    }
    return *it;
}


std::vector<std::string> PkModelCollection::getPkModelIdList() const
{
    std::vector<std::string> ids;
    std::transform(
            m_collection.begin(),
            m_collection.end(),
            std::back_inserter(ids), // Backward iterator on returned vector
            [](const std::shared_ptr<PkModel>& _mod) { return _mod->getPkModelId(); }); // Push values in vector
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
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_COLLECTION, _COMP_NO_NUM, _RC)                       \
    do {                                                                                              \
        {                                                                                             \
            std::shared_ptr<PkModel> pkmodel = std::make_shared<PkModel>(                             \
                    "linear.2comp.erlang" #_COMP_NO_NUM ".micro", PkModel::AllowMultipleRoutes::Yes); \
            rc |= pkmodel->addIntakeIntervalCalculatorFactory(                                        \
                    AbsorptionModel::Extravascular,                                                   \
                    Tucuxi::Core::RK4TwoCompartmentErlangMicro<_COMP_NO_NUM>::getCreator());          \
            rc |= pkmodel->addParameterList(                                                          \
                    AbsorptionModel::Extravascular,                                                   \
                    Tucuxi::Core::RK4TwoCompartmentErlangMicro<_COMP_NO_NUM>::getParametersId());     \
            Tucuxi::Common::TranslatableString distribution;                                          \
            Tucuxi::Common::TranslatableString elimination;                                           \
            std::string comps;                                                                        \
            comps = "compartments";                                                                   \
            distribution.setString(                                                                   \
                    std::to_string(2) + " " + comps                                                   \
                            + ", erlang absorption with " #_COMP_NO_NUM " transit compartments",      \
                    "en");                                                                            \
            elimination.setString("linear", "en");                                                    \
            pkmodel->setDistribution(distribution);                                                   \
            pkmodel->setElimination(elimination);                                                     \
            _collection.addPkModel(pkmodel);                                                          \
        }                                                                                             \
        {                                                                                             \
            std::shared_ptr<PkModel> pkmodel = std::make_shared<PkModel>(                             \
                    "linear.2comp.erlang" #_COMP_NO_NUM ".macro", PkModel::AllowMultipleRoutes::Yes); \
            rc |= pkmodel->addIntakeIntervalCalculatorFactory(                                        \
                    AbsorptionModel::Extravascular,                                                   \
                    Tucuxi::Core::RK4TwoCompartmentErlangMacro<_COMP_NO_NUM>::getCreator());          \
            rc |= pkmodel->addParameterList(                                                          \
                    AbsorptionModel::Extravascular,                                                   \
                    Tucuxi::Core::RK4TwoCompartmentErlangMacro<_COMP_NO_NUM>::getParametersId());     \
            Tucuxi::Common::TranslatableString distribution;                                          \
            Tucuxi::Common::TranslatableString elimination;                                           \
            std::string comps;                                                                        \
            comps = "compartments";                                                                   \
            distribution.setString(                                                                   \
                    std::to_string(2) + " " + comps                                                   \
                            + ", erlang absorption with " #_COMP_NO_NUM " transit compartments",      \
                    "en");                                                                            \
            elimination.setString("linear", "en");                                                    \
            pkmodel->setDistribution(distribution);                                                   \
            pkmodel->setElimination(elimination);                                                     \
            _collection.addPkModel(pkmodel);                                                          \
        }                                                                                             \
    } while (0);


/// \brief Add a PkModel with Lag time to a collection.
/// \param _COLLECTION Collection to which the PkModel has to be added.
/// \param _COMP_NO_NUM Number of components (expressed in numerical form).
/// \param _COMP_NO_LIT Number of components (expressed in literal form).
/// \param _TYPE Type (either Micro or Macro).
/// \param _TYPE_NAME Type in literal form (either micro or macro).
/// \param _RC Boolean return type (ORed result of all the add operations).
///
/// The lag time model is built in rk4
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ADD_PKMODEL_TO_COLLECTION_LAG_RK4(_COLLECTION, _COMP_NO_NUM, _COMP_NO_LIT, _TYPE, _TYPE_NAME, _RC)             \
    do {                                                                                                               \
        std::shared_ptr<PkModel> pkmodel = std::make_shared<PkModel>(                                                  \
                "linear." #_COMP_NO_NUM "comp." #_TYPE_NAME, PkModel::AllowMultipleRoutes::No);                        \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Extravascular, Tucuxi::Core::_COMP_NO_LIT##CompartmentExtra##_TYPE::getCreator());    \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::ExtravascularLag,                                                                     \
                Tucuxi::Core::RK4##_COMP_NO_LIT##CompartmentExtraLag##_TYPE::getCreator());                            \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Intravascular, Tucuxi::Core::_COMP_NO_LIT##CompartmentBolus##_TYPE::getCreator());    \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Infusion, Tucuxi::Core::_COMP_NO_LIT##CompartmentInfusion##_TYPE::getCreator());      \
        (_RC) |= pkmodel->addParameterList(                                                                            \
                AbsorptionModel::Extravascular,                                                                        \
                Tucuxi::Core::_COMP_NO_LIT##CompartmentExtra##_TYPE::getParametersId());                               \
        (_RC) |= pkmodel->addParameterList(                                                                            \
                AbsorptionModel::ExtravascularLag,                                                                     \
                Tucuxi::Core::RK4##_COMP_NO_LIT##CompartmentExtraLag##_TYPE::getParametersId());                       \
        (_RC) |= pkmodel->addParameterList(                                                                            \
                AbsorptionModel::Intravascular,                                                                        \
                Tucuxi::Core::_COMP_NO_LIT##CompartmentBolus##_TYPE::getParametersId());                               \
        (_RC) |= pkmodel->addParameterList(                                                                            \
                AbsorptionModel::Infusion, Tucuxi::Core::_COMP_NO_LIT##CompartmentInfusion##_TYPE::getParametersId()); \
        Tucuxi::Common::TranslatableString distribution;                                                               \
        Tucuxi::Common::TranslatableString elimination;                                                                \
        std::string comps;                                                                                             \
        if ((_COMP_NO_NUM) == 1)                                                                                       \
            comps = "compartment";                                                                                     \
        else                                                                                                           \
            comps = "compartments";                                                                                    \
        distribution.setString(std::to_string(_COMP_NO_NUM) + " " + comps, "en");                                      \
        elimination.setString("linear", "en");                                                                         \
        pkmodel->setDistribution(distribution);                                                                        \
        pkmodel->setElimination(elimination);                                                                          \
        (_COLLECTION).addPkModel(pkmodel);                                                                             \
    } while (0);

bool defaultPopulate(PkModelCollection& _collection)
{
    bool rc = true;

    ADD_PKMODEL_TO_COLLECTION_LAG(_collection, 1, One, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION_LAG(_collection, 1, One, Micro, micro, rc);
    ADD_PKMODEL_TO_COLLECTION_LAG(_collection, 2, Two, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION_LAG(_collection, 2, Two, MacroRatios, macroRatios, rc);
    ADD_PKMODEL_TO_COLLECTION_LAG(_collection, 2, Two, Micro, micro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 3, Three, Macro, macro, rc);
    ADD_PKMODEL_TO_COLLECTION(_collection, 3, Three, Micro, micro, rc);

    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 1, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 2, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 3, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 4, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 5, rc);
    ADD_2COMP_ERLANG_PK_MODEL_TO_COLLECTION(_collection, 6, rc);

    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("michaelismenten.1comp", PkModel::AllowMultipleRoutes::Yes);

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, RkMichaelisMentenOneCompExtra::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, RkMichaelisMentenOneCompExtra::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Intravascular, RkMichaelisMentenOneCompBolus::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Intravascular, RkMichaelisMentenOneCompBolus::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Infusion, RkMichaelisMentenOneCompInfusion::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Infusion, RkMichaelisMentenOneCompInfusion::getParametersId());

        Tucuxi::Common::TranslatableString elimination;
        elimination.setString("Michaelis-Menten", "en");
        sharedPkModel->setElimination(elimination);

        Tucuxi::Common::TranslatableString distribution;
        distribution.setString("Extra- or intra-vascular", "en");
        sharedPkModel->setDistribution(distribution);

        _collection.addPkModel(sharedPkModel);
    }

    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("michaelismenten.1comp.vmaxamount", PkModel::AllowMultipleRoutes::Yes);

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, RkMichaelisMentenOneCompVmaxAmountExtra::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, RkMichaelisMentenOneCompVmaxAmountExtra::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Intravascular, RkMichaelisMentenOneCompVmaxAmountBolus::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Intravascular, RkMichaelisMentenOneCompVmaxAmountBolus::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Infusion, RkMichaelisMentenOneCompVmaxAmountInfusion::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Infusion, RkMichaelisMentenOneCompVmaxAmountInfusion::getParametersId());

        Tucuxi::Common::TranslatableString elimination;
        elimination.setString("Michaelis-Menten", "en");
        sharedPkModel->setElimination(elimination);

        Tucuxi::Common::TranslatableString distribution;
        distribution.setString("Extra- or intra-vascular", "en");
        sharedPkModel->setDistribution(distribution);

        _collection.addPkModel(sharedPkModel);
    }

    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("michaelismenten.2comp.micro", PkModel::AllowMultipleRoutes::Yes);

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, RkMichaelisMentenTwoCompExtraMicro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, RkMichaelisMentenTwoCompExtraMicro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::ExtravascularLag, RkMichaelisMentenTwoCompExtraLagMicro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::ExtravascularLag, RkMichaelisMentenTwoCompExtraLagMicro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Intravascular, RkMichaelisMentenTwoCompBolusMicro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Intravascular, RkMichaelisMentenTwoCompBolusMicro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Infusion, RkMichaelisMentenTwoCompInfusionMicro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Infusion, RkMichaelisMentenTwoCompInfusionMicro::getParametersId());

        Tucuxi::Common::TranslatableString elimination;
        elimination.setString("Michaelis-Menten", "en");
        sharedPkModel->setElimination(elimination);

        Tucuxi::Common::TranslatableString distribution;
        distribution.setString("Extra- or intra-vascular", "en");
        sharedPkModel->setDistribution(distribution);

        _collection.addPkModel(sharedPkModel);
    }

    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("michaelismenten.2comp.macro", PkModel::AllowMultipleRoutes::Yes);

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, RkMichaelisMentenTwoCompExtraMacro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, RkMichaelisMentenTwoCompExtraMacro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::ExtravascularLag, RkMichaelisMentenTwoCompExtraLagMacro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::ExtravascularLag, RkMichaelisMentenTwoCompExtraLagMacro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Intravascular, RkMichaelisMentenTwoCompBolusMacro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Intravascular, RkMichaelisMentenTwoCompBolusMacro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Infusion, RkMichaelisMentenTwoCompInfusionMacro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Infusion, RkMichaelisMentenTwoCompInfusionMacro::getParametersId());

        Tucuxi::Common::TranslatableString elimination;
        elimination.setString("Michaelis-Menten", "en");
        sharedPkModel->setElimination(elimination);

        Tucuxi::Common::TranslatableString distribution;
        distribution.setString("Extra- or intra-vascular", "en");
        sharedPkModel->setDistribution(distribution);

        _collection.addPkModel(sharedPkModel);
    }


    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel =
                std::make_shared<PkModel>("michaelismenten.2comp.vmaxamount.micro", PkModel::AllowMultipleRoutes::Yes);

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, RkMichaelisMentenTwoCompVmaxAmountExtraMicro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, RkMichaelisMentenTwoCompVmaxAmountExtraMicro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::ExtravascularLag, RkMichaelisMentenTwoCompVmaxAmountExtraLagMicro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::ExtravascularLag, RkMichaelisMentenTwoCompVmaxAmountExtraLagMicro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Intravascular, RkMichaelisMentenTwoCompVmaxAmountBolusMicro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Intravascular, RkMichaelisMentenTwoCompVmaxAmountBolusMicro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Infusion, RkMichaelisMentenTwoCompVmaxAmountInfusionMicro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Infusion, RkMichaelisMentenTwoCompVmaxAmountInfusionMicro::getParametersId());

        Tucuxi::Common::TranslatableString elimination;
        elimination.setString("Michaelis-Menten", "en");
        sharedPkModel->setElimination(elimination);

        Tucuxi::Common::TranslatableString distribution;
        distribution.setString("Extra- or intra-vascular", "en");
        sharedPkModel->setDistribution(distribution);

        _collection.addPkModel(sharedPkModel);
    }

    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel =
                std::make_shared<PkModel>("michaelismenten.2comp.vmaxamount.macro", PkModel::AllowMultipleRoutes::Yes);

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, RkMichaelisMentenTwoCompVmaxAmountExtraMacro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, RkMichaelisMentenTwoCompVmaxAmountExtraMacro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::ExtravascularLag, RkMichaelisMentenTwoCompVmaxAmountExtraLagMacro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::ExtravascularLag, RkMichaelisMentenTwoCompVmaxAmountExtraLagMacro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Intravascular, RkMichaelisMentenTwoCompVmaxAmountBolusMacro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Intravascular, RkMichaelisMentenTwoCompVmaxAmountBolusMacro::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Infusion, RkMichaelisMentenTwoCompVmaxAmountInfusionMacro::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Infusion, RkMichaelisMentenTwoCompVmaxAmountInfusionMacro::getParametersId());

        Tucuxi::Common::TranslatableString elimination;
        elimination.setString("Michaelis-Menten", "en");
        sharedPkModel->setElimination(elimination);

        Tucuxi::Common::TranslatableString distribution;
        distribution.setString("Extra- or intra-vascular", "en");
        sharedPkModel->setDistribution(distribution);

        _collection.addPkModel(sharedPkModel);
    }

    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("michaelismenten.enzyme.1comp", PkModel::AllowMultipleRoutes::Yes);

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, RkMichaelisMentenEnzymeExtra::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, RkMichaelisMentenEnzymeExtra::getParametersId());

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Intravascular, RkMichaelisMentenEnzymeBolus::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Intravascular, RkMichaelisMentenEnzymeBolus::getParametersId());

        // rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(AbsorptionModel::Infusion, RkMichaelisMentenEnzymeInfusion::getCreator());
        // rc &= sharedPkModel->addParameterList(AbsorptionModel::Infusion, RkMichaelisMentenEnzymeInfusion::getParametersId());

        Tucuxi::Common::TranslatableString elimination;
        elimination.setString("Michaelis-Menten", "en");
        sharedPkModel->setElimination(elimination);

        Tucuxi::Common::TranslatableString distribution;
        distribution.setString("Extra- or intra-vascular, transit compartments", "en");
        sharedPkModel->setDistribution(distribution);

        _collection.addPkModel(sharedPkModel);
    }


#ifdef DRUGMODELTESTS
    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.constantelimination");

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, ConstantEliminationBolus::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, Tucuxi::Core::ConstantEliminationBolus::getParametersId());

        _collection.addPkModel(sharedPkModel);
    }
    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.pkasymptotic");

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, PkAsymptotic::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, Tucuxi::Core::PkAsymptotic::getParametersId());

        _collection.addPkModel(sharedPkModel);
    }
    {
        std::shared_ptr<PkModel> sharedPkModel;
        sharedPkModel = std::make_shared<PkModel>("test.multiconstantelimination");

        rc &= sharedPkModel->addIntakeIntervalCalculatorFactory(
                AbsorptionModel::Extravascular, MultiConstantEliminationBolus::getCreator());
        rc &= sharedPkModel->addParameterList(
                AbsorptionModel::Extravascular, Tucuxi::Core::MultiConstantEliminationBolus::getParametersId());

        _collection.addPkModel(sharedPkModel);
    }
#endif // DRUGMODELTESTS

    return rc;
}

} // namespace Core
} // namespace Tucuxi
