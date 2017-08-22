/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_PKMODEL_H
#define TUCUXI_CORE_PKMODEL_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/onecompartmentbolus.h"
#include "tucucore/onecompartmentextra.h"
#include "tucucore/onecompartmentinfusion.h"
#include "tucucore/twocompartmentbolus.h"
#include "tucucore/twocompartmentextra.h"
#include "tucucore/twocompartmentinfusion.h"
#include "tucucore/threecompartmentbolus.h"
#include "tucucore/threecompartmentextra.h"
#include "tucucore/threecompartmentinfusion.h"

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
    /// \brief Create a PkModel, setting its Id.
    /// \param _pkModelId the Id to set.
    /// \warning The ID shall be unique within the software
    PkModel(const std::string &_pkModelId);

    /// \brief Get the PkModel Id.
    /// \return ID of the PkModel
    std::string getPkModelId() const;

    /// \brief Add an IntakeIntervalCalculatorCreator (so that we will be able to retrieve it when needed).
    /// \param _route Route of administration for the specific calculator.
    /// \param _creator A creator able to create a calculator of a specific class.
    /// \return True if the insertion was performed successfully, false if the route already had an associated creator.
    ///         In either cases, the creator is recorded.
    bool addIntakeIntervalCalculatorFactory(const RouteOfAdministration _route,
                                            std::shared_ptr<IntakeIntervalCalculatorCreator> _creator);

    /// \brief Get the available routes of administration.
    /// \return A vector of supported routes of administrations.
    std::vector<RouteOfAdministration> getAvailableRoutes() const;

    /// \brief Retrieve the calculator for a specific route of administration.
    /// \param _route Route of administration for which an intake calculator is requested.
    /// \return An instance of an IntakeIntervalCalculator responsible for the specific route if available, nullptr
    ///         otherwise.
    std::shared_ptr<IntakeIntervalCalculator> getCalculatorForRoute(const RouteOfAdministration _route) const;


protected:
    /// \brief Identifier of the PkModel.
    std::string m_pkModelId;

    /// \brief A map of available calculators creators linked to the route of administration.
    std::map<RouteOfAdministration, std::shared_ptr<IntakeIntervalCalculatorCreator>> m_calculatorCreators;
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
    bool addPkModel(std::shared_ptr<PkModel> _pkModel);

    /// \brief Given an Id, returns the PkModel with the corresponding identifier.
    /// \param _pkModelId ID of the PkModel asked by the caller.
    /// \return A PkModel corresponding to the ID if available, nullptr if no PkModel is found for the specified ID.
    std::shared_ptr<PkModel> getPkModelFromId(const std::string &_pkModelId) const;

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
    friend bool populate(PkModelCollection &_collection);

protected:
    std::vector<std::shared_ptr<PkModel>> m_collection;
};


}
}

#endif // TUCUXI_CORE_PKMODEL_H
