//@@license@@

#ifndef TUCUXI_CORE_PKMODEL_H
#define TUCUXI_CORE_PKMODEL_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "tucucommon/translatablestring.h"

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

class IntakeIntervalCalculator;
class IntakeIntervalCalculatorCreator;

/// \brief Pharmacokinetic model -- way of calculating predictions for a particular body model.
/// This class is meant to be uniquely identified by an ID, and to
/// allow linking the IntakeIntervalCalculator implementing a specific PK model.
///
class PkModel
{
public:
    enum class [[nodiscard]] AllowMultipleRoutes{No = 0, Yes};

    /// \brief Create a PkModel, setting its Id.
    /// \param _pkModelId the Id to set.
    /// \warning The ID shall be unique within the software
    PkModel(std::string _pkModelId, AllowMultipleRoutes _allowMultipleRoutes = PkModel::AllowMultipleRoutes::No);

    /// \brief Get the PkModel Id.
    /// \return ID of the PkModel
    std::string getPkModelId() const;

    /// \brief Add an IntakeIntervalCalculatorCreator (so that we will be able to retrieve it when needed).
    /// \param _route Route of administration for the specific calculator.
    /// \param _creator A creator able to create a calculator of a specific class.
    /// \return True if the insertion was performed successfully, false if the route already had an associated creator.
    ///         In either cases, the creator is recorded.
    bool addIntakeIntervalCalculatorFactory(
            AbsorptionModel _route, const std::shared_ptr<IntakeIntervalCalculatorCreator>& _creator);

    bool addParameterList(AbsorptionModel _route, std::vector<std::string> _parameterList);

    /// \brief Get the available routes of administration.
    /// \return A vector of supported routes of administrations.
    std::vector<AbsorptionModel> getAvailableRoutes() const;

    /// \brief Retrieve the calculator for a specific route of administration.
    /// \param _route Route of administration for which an intake calculator is requested.
    /// \return An instance of an IntakeIntervalCalculator responsible for the specific route if available, nullptr
    ///         otherwise.
    std::shared_ptr<IntakeIntervalCalculator> getCalculatorForRoute(AbsorptionModel _route) const;

    std::vector<std::string> getParametersForRoute(AbsorptionModel _route) const;

    const Tucuxi::Common::TranslatableString& getDistribution() const
    {
        return m_distribution;
    }
    const Tucuxi::Common::TranslatableString& getElimination() const
    {
        return m_elimination;
    }

    void setDistribution(const Tucuxi::Common::TranslatableString& _distribution)
    {
        m_distribution = _distribution;
    }
    void setElimination(const Tucuxi::Common::TranslatableString& _elimination)
    {
        m_elimination = _elimination;
    }

    /// \brief Indicates if multiple routes can be managed in a single dosage history
    /// \return AllowMultipleRoutes::Yes if multiple routes can be combined in a a DosageHistory, AllowMultipleRoutes::No else
    ///
    /// Not all PkModels allow for multiple routes, therefore it is important to check that before starting a
    /// computation.
    AllowMultipleRoutes allowMultipleRoutes() const
    {
        return m_allowMultipleRoutes;
    }

    ///
    /// \brief Gets all the PK model compartment infos
    /// \return The vector of CompartmentInfo
    ///
    /// This method is useful to know what does the results of a concentration calculation represent in
    /// case of a multi-analytes PK model or if we ask for all compartments to be returned as results.
    ///
    const std::vector<CompartmentInfo>& getCompartmentInfos() const
    {
        return m_compartmentInfos;
    }

protected:
    /// \brief Identifier of the PkModel.
    std::string m_pkModelId;

    /// \brief A map of available calculators creators linked to the route of administration.
    std::map<AbsorptionModel, std::shared_ptr<IntakeIntervalCalculatorCreator>> m_calculatorCreators;

    /// \brief A map of required PK parameters linked to the route of administration.
    std::map<AbsorptionModel, std::vector<std::string>> m_parameters;

    Tucuxi::Common::TranslatableString m_distribution;
    Tucuxi::Common::TranslatableString m_elimination;

    /// \brief Indicates if multiple routes can be managed in a single dosage history
    const AllowMultipleRoutes m_allowMultipleRoutes;

    /// \brief Compartment infos. To be set soon.
    const std::vector<CompartmentInfo> m_compartmentInfos;
};


/// \brief Collection of PkModels -- groups PkModels so that they can be easily retrieved.
class PkModelCollection
{
public:
    /// \brief Initialize an empty PkModel collection.
    PkModelCollection() = default;

    /// \brief Add a PkModel to the collection.
    /// \param _pkModel A pointer to a PkModel to add to the collection.
    /// This function is typically used at startup to populate the collection with the existing models of computation.
    bool addPkModel(const std::shared_ptr<Core::PkModel>& _pkModel);

    /// \brief Given an Id, returns the PkModel with the corresponding identifier.
    /// \param _pkModelId ID of the PkModel asked by the caller.
    /// \return A PkModel corresponding to the ID if available, nullptr if no PkModel is found for the specified ID.
    std::shared_ptr<PkModel> getPkModelFromId(const std::string& _pkModelId) const;

    /// \brief Retrieve the list of PkModel Id's available in the collection.
    /// \return List of PkModel IDs in the collection.
    std::vector<std::string> getPkModelIdList() const;

    /// \brief Retrieve the set of PkModels contained in the collection.
    /// \return Vector containing all PkModels of the collection.
    std::vector<std::shared_ptr<PkModel>> getPkModelList() const;

    /// \brief Adds all available PkModels to a collection.
    /// \param _collection The collection to populate.
    /// \return True if the collection has been successfully populated, false otherwise.
    /// This method shall be called at the beginning of the execution.
    /// \todo The collection is currently hardcoded -- ideally it should be read from an XML file.
    friend bool populate(PkModelCollection& _collection);

protected:
    std::vector<std::shared_ptr<PkModel>> m_collection;
};

bool defaultPopulate(PkModelCollection& _collection);

/// \brief Add a PkModel to a collection.
/// \param _COLLECTION Collection to which the PkModel has to be added.
/// \param _COMP_NO_NUM Number of components (expressed in numerical form).
/// \param _COMP_NO_LIT Number of components (expressed in literal form).
/// \param _TYPE Type (either Micro or Macro).
/// \param _TYPE_NAME Type in literal form (either micro or macro).
/// \param _RC Boolean return type (ORed result of all the add operations).
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ADD_PKMODEL_TO_COLLECTION(_COLLECTION, _COMP_NO_NUM, _COMP_NO_LIT, _TYPE, _TYPE_NAME, _RC)                     \
    do {                                                                                                               \
        std::shared_ptr<PkModel> pkmodel = std::make_shared<PkModel>(                                                  \
                "linear." #_COMP_NO_NUM "comp." #_TYPE_NAME, PkModel::AllowMultipleRoutes::No);                        \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Extravascular, Tucuxi::Core::_COMP_NO_LIT##CompartmentExtra##_TYPE::getCreator());    \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Intravascular, Tucuxi::Core::_COMP_NO_LIT##CompartmentBolus##_TYPE::getCreator());    \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Infusion, Tucuxi::Core::_COMP_NO_LIT##CompartmentInfusion##_TYPE::getCreator());      \
        (_RC) |= pkmodel->addParameterList(                                                                            \
                AbsorptionModel::Extravascular,                                                                        \
                Tucuxi::Core::_COMP_NO_LIT##CompartmentExtra##_TYPE::getParametersId());                               \
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


/// \brief Add a PkModel with Lag time to a collection.
/// \param _COLLECTION Collection to which the PkModel has to be added.
/// \param _COMP_NO_NUM Number of components (expressed in numerical form).
/// \param _COMP_NO_LIT Number of components (expressed in literal form).
/// \param _TYPE Type (either Micro or Macro).
/// \param _TYPE_NAME Type in literal form (either micro or macro).
/// \param _RC Boolean return type (ORed result of all the add operations).
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ADD_PKMODEL_TO_COLLECTION_LAG(_COLLECTION, _COMP_NO_NUM, _COMP_NO_LIT, _TYPE, _TYPE_NAME, _RC)                 \
    do {                                                                                                               \
        std::shared_ptr<PkModel> pkmodel = std::make_shared<PkModel>(                                                  \
                "linear." #_COMP_NO_NUM "comp." #_TYPE_NAME, PkModel::AllowMultipleRoutes::No);                        \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Extravascular, Tucuxi::Core::_COMP_NO_LIT##CompartmentExtra##_TYPE::getCreator());    \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::ExtravascularLag,                                                                     \
                Tucuxi::Core::_COMP_NO_LIT##CompartmentExtraLag##_TYPE::getCreator());                                 \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Intravascular, Tucuxi::Core::_COMP_NO_LIT##CompartmentBolus##_TYPE::getCreator());    \
        (_RC) |= pkmodel->addIntakeIntervalCalculatorFactory(                                                          \
                AbsorptionModel::Infusion, Tucuxi::Core::_COMP_NO_LIT##CompartmentInfusion##_TYPE::getCreator());      \
        (_RC) |= pkmodel->addParameterList(                                                                            \
                AbsorptionModel::Extravascular,                                                                        \
                Tucuxi::Core::_COMP_NO_LIT##CompartmentExtra##_TYPE::getParametersId());                               \
        (_RC) |= pkmodel->addParameterList(                                                                            \
                AbsorptionModel::ExtravascularLag,                                                                     \
                Tucuxi::Core::_COMP_NO_LIT##CompartmentExtraLag##_TYPE::getParametersId());                            \
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


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_PKMODEL_H
