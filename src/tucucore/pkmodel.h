/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_PKMODEL_H
#define TUCUXI_CORE_PKMODEL_H

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

class IntakeIntervalCalculator;
class IntakeIntervalCalculatorCreator;


///
/// \brief The PkModel class
/// This class is meant to be uniquely identified by an ID, and to
/// allow linking the IntakeIntervalCalculator implementing a specific PK model.
///
class PkModel
{
public:
    PkModel();

    ///
    /// \brief setPkModelId Sets the ID of the PkModel
    /// \param _pkModelId the Id to set
    ///
    void setPkModelId(std::string _pkModelId);

    ///
    /// \brief getPkModelId
    /// \return the ID of the PkModel
    /// The ID shall be unique within the software
    ///
    std::string getPkModelId() const;

    ///
    /// \brief addIntakeIntervalCalculatorFactory
    /// \param _route Route of administration for the specific calculator
    /// \param _creator A creator able to create a calculator of a specific class
    ///
    void addIntakeIntervalCalculatorFactory(RouteOfAdministration _route, IntakeIntervalCalculatorCreator *_creator);
    ///
    /// \brief getAvailableRoutes
    /// \return A vector of supported routes of administrations
    ///
    std::vector<RouteOfAdministration> getAvailableRoutes() const;

    ///
    /// \brief getCalculatorForRoute
    /// \param _route Route of administration for which an intake calculator is requested
    /// \return An instance of an IntakeIntervalCalculator responsible for the specific route
    /// It uses the factories in order to create specific calculators
    /// Returns nullptr if no calculator is available for the specific route.
    ///
    IntakeIntervalCalculator *getCalculatorForRoute(RouteOfAdministration _route) const;

protected:

    /// \brief Id of the PkModel
    std::string m_pkModelId;

    /// \brief A map of available calculators creators linked to there route of administration
    std::map<RouteOfAdministration, IntakeIntervalCalculatorCreator* > m_calculatorCreators;
};

class PkModelCollection
{
public:
    PkModelCollection();

    ///
    /// \brief addPkModel
    /// \param _pkModel a PkModel to add to the collection
    /// This function is typically used at startup to populate the collection with the existing models of computation
    ///
    void addPkModel(PkModel *_pkModel);

    ///
    /// \brief getPkModelFromId
    /// \param _pkModelId ID of the PkModel asked by the caller
    /// \return A PkModel corresponding to the ID
    /// Returns nullptr if no PkModel is found for the specified ID
    PkModel *getPkModelFromId(std::string _pkModelId) const;

    ///
    /// \brief getPkModelIdList
    /// \return the list of PkModel IDs in the collection
    ///
    std::vector<std::string> getPkModelIdList() const;

    ///
    /// \brief getPkModelList
    /// \return a vector containing all PkModel of the collection
    ///
    std::vector<PkModel* > getPkModelList() const;
};

class PkModelCollectionPopulator
{
public:
    PkModelCollectionPopulator();

    ///
    /// \brief populate Adds all available PkModels to a collection
    /// \param _collection The collection to populate
    /// This method shall be called at the beginning of the execution
    void populate(PkModelCollection &_collection);
};

}
}

#endif // TUCUXI_CORE_PKMODEL_H
