/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef DRUGMODELIMPORT_H
#define DRUGMODELIMPORT_H

#include <mutex>
#include <string>
#include <vector>

#include "tucucommon/translatablestring.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmlnode.h"

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/errormodel.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugmodel/parameterdefinition.h"


namespace Tucuxi {
namespace Core {

enum class CovariateType;
enum class DataType;
enum class InterpolationType;
enum class TargetType;

class DrugModel;
class TimeConsiderations;
class HalfLife;
class OutdatedMeasure;
class PopulationValue;
class Operation;
class DrugModelDomain;
class Constraint;
class JSOperation;
class CovariateDefinition;
class ActiveMoiety;
class TargetDefinition;
class AnalyteSet;
class Analyte;
class MolarMass;
class ErrorModel;
class ParameterSetDefinition;
class ParameterDefinition;
class Correlation;
class FullFormulationAndRoute;
class FormulationAndRoutes;
class DrugModelMetadata;

class LightPopulationValue
{
public:
    LightPopulationValue() : m_operation(nullptr), m_value(0.0) {}

    std::unique_ptr<Operation> getOperation()
    {
        return std::move(m_operation);
    }
    Value getValue()
    {
        return m_value;
    }

    std::unique_ptr<Operation> m_operation;
    Value m_value;
};


class DrugModelImport : public Tucuxi::Common::XMLImporter
{
public:
    ///
    /// \brief DrugModelImport empty constructor
    ///
    DrugModelImport();

    virtual ~DrugModelImport() = default;

    ///
    /// \brief importFromFile
    /// \param _drugModel A reference to a DrugModel unique pointer that will be allocated within the function
    /// \param _fileName The name of the file in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importFromFile(std::unique_ptr<Tucuxi::Core::DrugModel>& _drugModel, const std::string& _fileName);

    ///
    /// \brief importFromString
    /// \param _drugModel A reference to a DrugModel unique pointer that will be allocated within the function
    /// \param _xml A string in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importFromString(std::unique_ptr<DrugModel>& _drugModel, const std::string& _xml);

    ///
    /// \brief importOperationFromFile
    /// \param _operation A reference to an Operation unique pointer that will be allocated within the function
    /// \param _fileName The name of the file in which the operation is stored
    /// \return Result_Ok if the import went well, another Result else.
    /// This function is reentrant.
    /// It is meant to be used by the the DrugModelChecker to help the online drug model editor.
    ///
    DrugModelImport::Status importOperationFromFile(
            std::unique_ptr<Tucuxi::Core::Operation>& _operation, const std::string& _fileName);

    ///
    /// \brief importOperationFromString
    /// \param _operation A reference to an Operation unique pointer that will be allocated within the function
    /// \param _xml A string in which the operation is stored
    /// \return Result_Ok if the import went well, another Result else.
    /// This function is reentrant.
    /// It is meant to be used by the the DrugModelChecker to help the online drug model editor.
    ///
    DrugModelImport::Status importOperationFromString(
            std::unique_ptr<Tucuxi::Core::Operation>& _operation, const std::string& _xml);

protected:
    const std::vector<std::string>& ignoredTags() const override;

    /// A mutex to ensure the public methods are reentrant
    std::mutex m_mutex;

    ///
    /// \brief importFromString
    /// \param _drugModel A reference to a DrugModel unique pointer that will be allocated within the function
    /// \param _document An XmlDocument in which the drug model is stored
    /// \return Result::Ok if the import went well, another Result else.
    /// This function is reentrant.
    ///
    Status importDocument(std::unique_ptr<DrugModel>& _drugModel, Tucuxi::Common::XmlDocument& _document);


    ///////////////////////////////////////////////////////////////////////////////
    /// enum class imports
    ///////////////////////////////////////////////////////////////////////////////

    CovariateType extractCovariateType(Tucuxi::Common::XmlNodeIterator _node);
    DataType extractDataType(Tucuxi::Common::XmlNodeIterator _node);
    InterpolationType extractInterpolationType(Tucuxi::Common::XmlNodeIterator _node);
    TargetType extractTargetType(Tucuxi::Common::XmlNodeIterator _node);
    ResidualErrorType extractResidualErrorType(Tucuxi::Common::XmlNodeIterator _node);
    ParameterVariabilityType extractParameterVariabilityType(Tucuxi::Common::XmlNodeIterator _node);
    Formulation extractFormulation(Tucuxi::Common::XmlNodeIterator _node);
    AdministrationRoute extractAdministrationRoute(Tucuxi::Common::XmlNodeIterator _node);
    AbsorptionModel extractAbsorptionModel(Tucuxi::Common::XmlNodeIterator _node);


    ///////////////////////////////////////////////////////////////////////////////
    /// objects imports
    ///////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DrugModel> extractDrugModel(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<TimeConsiderations> extractTimeConsiderations(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<HalfLife> extractHalfLife(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<OutdatedMeasure> extractOutdatedMeasure(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<DrugModelDomain> extractDrugDomain(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<std::unique_ptr<Constraint> > extractConstraints(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<Constraint> extractConstraint(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<std::unique_ptr<ActiveMoiety> > extractActiveMoieties(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<ActiveMoiety> extractActiveMoiety(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<std::unique_ptr<TargetDefinition> > extractTargets(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<TargetDefinition> extractTarget(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<std::unique_ptr<AnalyteSet> > extractAnalyteGroups(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<AnalyteSet> extractAnalyteGroup(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<std::unique_ptr<Analyte> > extractAnalytes(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<Analyte> extractAnalyte(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<MolarMass> extractMolarMass(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<ErrorModel> extractErrorModel(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<ParameterSetDefinition> extractParameterSet(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<std::unique_ptr<ParameterDefinition> > extractParameters(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<ParameterDefinition> extractParameter(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<std::unique_ptr<Correlation> > extractCorrelations(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<Correlation> extractCorrelation(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<ParameterVariability> extractVariability(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<FormulationAndRoutes> extractFullFormulationAndRoutes(
            Tucuxi::Common::XmlNodeIterator _node, const std::vector<std::unique_ptr<AnalyteSet> >& _analyteSets);
    std::unique_ptr<FullFormulationAndRoute> extractFullFormulationAndRoute(
            Tucuxi::Common::XmlNodeIterator _node, const std::vector<std::unique_ptr<AnalyteSet> >& _analyteSets);
    std::unique_ptr<ValidDurations> extractValidDurations(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<ValidDoses> extractValidDoses(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<ValidValuesRange> extractValuesRange(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<ValidValuesFixed> extractValuesFixed(Tucuxi::Common::XmlNodeIterator _node);
    std::vector<std::unique_ptr<CovariateDefinition> > extractCovariates(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<CovariateDefinition> extractCovariate(Tucuxi::Common::XmlNodeIterator _node);
    std::unique_ptr<LightPopulationValue> extractPopulationValue(Tucuxi::Common::XmlNodeIterator _node);

    template<typename T>
    std::unique_ptr<Operation> extractOperation(T _node);
    std::unique_ptr<JSOperation> extractJSOperation(Tucuxi::Common::XmlNodeIterator _node);
    Tucuxi::Common::TranslatableString extractTranslatableString(
            Tucuxi::Common::XmlNodeIterator _node, const std::string& _insideName);
    std::unique_ptr<DrugModelMetadata> extractHead(Tucuxi::Common::XmlNodeIterator _node);
};


} // namespace Core
} // namespace Tucuxi

#endif // DRUGMODELIMPORT_H
