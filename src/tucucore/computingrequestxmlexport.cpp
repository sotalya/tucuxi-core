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

#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

#include "computingrequestxmlexport.h"

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"


namespace Tucuxi {
namespace Core {


std::string dateTimeToString(const Tucuxi::Common::DateTime& _dateTime)
{
    if (_dateTime.isUndefined()) {
        return "";
    }
    std::string result = Tucuxi::Common::Utils::strFormat(
            "%04d-%02d-%02dT%02d:%02d:%02d",
            _dateTime.year(),
            _dateTime.month(),
            _dateTime.day(),
            _dateTime.hour(),
            _dateTime.minute(),
            _dateTime.second());
    return result;
}

std::string timeToString(const Tucuxi::Common::TimeOfDay& _timeOfDay)
{
    return Tucuxi::Common::Utils::strFormat(
            "%02d:%02d:%02d", _timeOfDay.hour(), _timeOfDay.minute(), _timeOfDay.second());
}



ComputingRequestXmlExport::ComputingRequestXmlExport() = default;

ComputingRequestXmlExport::~ComputingRequestXmlExport() = default;

bool ComputingRequestXmlExport::exportToFile(const ComputingRequest& _computingRequest, const std::string& _fileName)
{
    std::string xmlString;
    if (!exportToString(_computingRequest, xmlString)) {
        return false;
    }

    std::ofstream file;
    file.open(_fileName);
    if ((file.rdstate() & std::ostream::failbit) != 0) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("The file {} could not be opened.", _fileName);
        return false;
    }
    file << xmlString;
    file.close();

    return true;
}


bool ComputingRequestXmlExport::exportToString(const ComputingRequest& _computingRequest, std::string& _xmlString)
{
    // Ensure the function is reentrant
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    Tucuxi::Common::XmlNode root = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "query");
    auto attribute0 = m_doc.createAttribute("version", "1.0");
    auto attribute1 = m_doc.createAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    auto attribute2 = m_doc.createAttribute("xsi:noNamespaceSchemaLocation", "computing_query.xsd");
    root.addAttribute(attribute0);
    root.addAttribute(attribute1);
    root.addAttribute(attribute2);

    m_doc.setRoot(root);
    Tucuxi::Common::XmlNode queryId =
            m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "queryId", _computingRequest.getId());
    root.addChild(queryId);

    Tucuxi::Common::XmlNode clientId = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "clientId", "clientId");
    root.addChild(clientId);

    Tucuxi::Common::XmlNode theDate =
            m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "date", dateTimeToString(DateTime::now()));
    root.addChild(theDate);

    Tucuxi::Common::XmlNode language = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "language", "en");
    root.addChild(language);


    Tucuxi::Common::XmlNode drugTreatment = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "drugTreatment");
    root.addChild(drugTreatment);

    Tucuxi::Common::XmlNode patient = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "patient");
    drugTreatment.addChild(patient);

    Tucuxi::Common::XmlNode covariates = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "covariates");
    patient.addChild(covariates);

    for (const auto& covariate : _computingRequest.getDrugTreatment().getCovariates()) {
        Tucuxi::Common::XmlNode covariateNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "covariate");
        covariates.addChild(covariateNode);
        addNode(covariateNode, "covariateId", covariate->getId());
        addNode(covariateNode, "date", dateTimeToString(covariate->getEventTime()));
        addNode(covariateNode, "value", covariate->getValue());
        addNode(covariateNode, "unit", covariate->getUnit().toString());
        addNode(covariateNode, "dataType", covariate->getDataType());
        // TODO : Change that. Should not always be continuous
        addNode(covariateNode, "nature", "continuous");
    }

    Tucuxi::Common::XmlNode drugs = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "drugs");
    drugTreatment.addChild(drugs);

    Tucuxi::Common::XmlNode drug = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "drug");
    drugs.addChild(drug);

    addNode(drug, "drugId", _computingRequest.getDrugModel().getDrugId());
    addNode(drug,
            "activePrinciple",
            _computingRequest.getDrugModel().getActiveMoieties()[0]->getActiveMoietyId().toString());
    // TODO : Change the brand name
    addNode(drug, "brandName", "brandName");
    // TODO : Change ATC
    addNode(drug, "atc", "atc");

    Tucuxi::Common::XmlNode treatment = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "treatment");
    drug.addChild(treatment);
    exportDosageHistory(_computingRequest.getDrugTreatment().getDosageHistory(), treatment);

    if (!_computingRequest.getDrugTreatment().getSamples().empty()) {
        Tucuxi::Common::XmlNode samples = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "samples");
        drug.addChild(samples);

        for (const auto& sample : _computingRequest.getDrugTreatment().getSamples()) {
            Tucuxi::Common::XmlNode sampleNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "sample");
            samples.addChild(sampleNode);
            addNode(sampleNode, "sampleId", "1");
            addNode(sampleNode, "sampleDate", dateTimeToString(sample->getDate()));
            Tucuxi::Common::XmlNode concentrations =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "concentrations");
            sampleNode.addChild(concentrations);
            Tucuxi::Common::XmlNode concentration =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "concentration");
            concentrations.addChild(concentration);
            addNode(concentration, "analyteId", sample->getAnalyteID().toString());
            addNode(concentration, "value", sample->getValue());
            addNode(concentration, "unit", sample->getUnit().toString());
        }
    }

    Tucuxi::Common::XmlNode targets = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "targets");
    drug.addChild(targets);

    for (const auto& target : _computingRequest.getDrugTreatment().getTargets()) {
        Tucuxi::Common::XmlNode targetNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "target");
        targets.addChild(targetNode);
        addNode(targetNode, "activeMoietyId", target->getActiveMoietyId().toString());
        addNode(targetNode, "targetType", target->getTargetType());
        addNode(targetNode, "unit", target->getUnit().toString());
        addNode(targetNode, "min", target->getValueMin());
        addNode(targetNode, "best", target->getValueBest());
        addNode(targetNode, "max", target->getValueMax());
        addNode(targetNode, "inefficacyAlarm", target->getInefficacyAlarm());
        addNode(targetNode, "toxicityAlarm", target->getToxicityAlarm());
        // TODO : We miss MIC-related fields here
    }

    Tucuxi::Common::XmlNode requests = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "requests");
    root.addChild(requests);



    for (const auto& trait : _computingRequest.getComputingTraits()) {



        Tucuxi::Common::XmlNode requestNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "request");
        requests.addChild(requestNode);


        addNode(requestNode, "requestId", trait->getId());
        addNode(requestNode, "drugId", _computingRequest.getDrugModel().getDrugId());
        addNode(requestNode, "drugModelId", _computingRequest.getDrugModel().getDrugModelId());

        if (dynamic_cast<const Tucuxi::Core::ComputingTraitConcentration*>(trait.get()) != nullptr) {

            auto predictionTrait = dynamic_cast<const Tucuxi::Core::ComputingTraitConcentration*>(trait.get());
            Tucuxi::Common::XmlNode prediction =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "predictionTraits");
            requestNode.addChild(prediction);

            exportComputingOption(predictionTrait->getComputingOption(), prediction);

            addNode(prediction, "nbPointsPerHour", static_cast<int>(predictionTrait->getNbPointsPerHour()));
            Tucuxi::Common::XmlNode dateInterval =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dateInterval");
            prediction.addChild(dateInterval);
            addNode(dateInterval, "start", dateTimeToString(predictionTrait->getStart()));
            addNode(dateInterval, "end", dateTimeToString(predictionTrait->getEnd()));
        }
        else if (dynamic_cast<const Tucuxi::Core::ComputingTraitPercentiles*>(trait.get()) != nullptr) {

            auto predictionTrait = dynamic_cast<const Tucuxi::Core::ComputingTraitPercentiles*>(trait.get());
            Tucuxi::Common::XmlNode prediction =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "percentilesTraits");
            requestNode.addChild(prediction);

            exportComputingOption(predictionTrait->getComputingOption(), prediction);

            addNode(prediction, "nbPointsPerHour", static_cast<int>(predictionTrait->getNbPointsPerHour()));
            Tucuxi::Common::XmlNode dateInterval =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dateInterval");
            prediction.addChild(dateInterval);
            addNode(dateInterval, "start", dateTimeToString(predictionTrait->getStart()));
            addNode(dateInterval, "end", dateTimeToString(predictionTrait->getEnd()));

            Tucuxi::Common::XmlNode ranks = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "ranks");
            prediction.addChild(ranks);
            for (const auto r : predictionTrait->getRanks()) {
                addNode(ranks, "rank", r);
            }
        }
        else if (dynamic_cast<const Tucuxi::Core::ComputingTraitAdjustment*>(trait.get()) != nullptr) {

            auto predictionTrait = dynamic_cast<const Tucuxi::Core::ComputingTraitAdjustment*>(trait.get());
            Tucuxi::Common::XmlNode prediction =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "adjustmentTraits");
            requestNode.addChild(prediction);

            exportComputingOption(predictionTrait->getComputingOption(), prediction);

            addNode(prediction, "nbPointsPerHour", static_cast<int>(predictionTrait->getNbPointsPerHour()));
            Tucuxi::Common::XmlNode dateInterval =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dateInterval");
            prediction.addChild(dateInterval);
            addNode(dateInterval, "start", dateTimeToString(predictionTrait->getStart()));
            addNode(dateInterval, "end", dateTimeToString(predictionTrait->getEnd()));

            addNode(prediction, "adjustmentDate", dateTimeToString(predictionTrait->getAdjustmentTime()));
            Tucuxi::Common::XmlNode options = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "options");
            prediction.addChild(options);
            addNode(options, "bestCandidatesOption", predictionTrait->getBestCandidatesOption());
            addNode(options, "loadingOption", predictionTrait->getLoadingOption());
            addNode(options, "restPeriodOption", predictionTrait->getRestPeriodOption());
            addNode(options, "steadyStateTargetOption", predictionTrait->getSteadyStateTargetOption());
            addNode(options, "targetExtractionOption", predictionTrait->getTargetExtractionOption());
            addNode(options,
                    "formulationAndRouteSelectionOption",
                    predictionTrait->getFormulationAndRouteSelectionOption());
        }
        else if (dynamic_cast<const Tucuxi::Core::ComputingTraitAtMeasures*>(trait.get()) != nullptr) {
            auto predictionTrait = dynamic_cast<const Tucuxi::Core::ComputingTraitAtMeasures*>(trait.get());
            Tucuxi::Common::XmlNode prediction =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "predictionAtSampleTimesTraits");
            requestNode.addChild(prediction);

            exportComputingOption(predictionTrait->getComputingOption(), prediction);
        }
        else if (dynamic_cast<const Tucuxi::Core::ComputingTraitSinglePoints*>(trait.get()) != nullptr) {
            auto predictionTrait = dynamic_cast<const Tucuxi::Core::ComputingTraitSinglePoints*>(trait.get());
            Tucuxi::Common::XmlNode prediction =
                    m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "predictionAtTimesTraits");
            requestNode.addChild(prediction);

            exportComputingOption(predictionTrait->getComputingOption(), prediction);

            Tucuxi::Common::XmlNode dates = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dates");
            prediction.addChild(dates);
            for (const auto& d : predictionTrait->getTimes()) {
                addNode(dates, "date", dateTimeToString(d));
            }
        }

        else {
            // TODO : Not supported export
        }
    }

    m_doc.toString(_xmlString, true);
    return true;
}


void ComputingRequestXmlExport::addNode(
        Tucuxi::Common::XmlNode& _rootNode, const std::string& _nodeName, const std::string& _nodeValue)
{
    Tucuxi::Common::XmlNode node = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, _nodeName, _nodeValue);
    _rootNode.addChild(node);
}

void ComputingRequestXmlExport::addNode(
        Tucuxi::Common::XmlNode& _rootNode, const std::string& _nodeName, const char* _nodeValue)
{
    Tucuxi::Common::XmlNode node = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, _nodeName, _nodeValue);
    _rootNode.addChild(node);
}




void ComputingRequestXmlExport::exportComputingOption(
        const Tucuxi::Core::ComputingOption& _option, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode computingOption =
            m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "computingOption");
    _rootNode.addChild(computingOption);
    addNode(computingOption, "parametersType", _option.getParametersType());
    addNode(computingOption, "compartmentOption", _option.getCompartmentsOption());
    addNode(computingOption, "retrieveStatistics", _option.retrieveStatistics());
    addNode(computingOption, "retrieveParameters", _option.retrieveParameters());
    addNode(computingOption, "retrieveCovariates", _option.retrieveCovariates());
}



bool ComputingRequestXmlExport::exportDosageTimeRange(
        const std::unique_ptr<Tucuxi::Core::DosageTimeRange>& _timeRange, Tucuxi::Common::XmlNode& _rootNode)
{

    Tucuxi::Common::XmlNode dosageTimeRange =
            m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dosageTimeRange");
    _rootNode.addChild(dosageTimeRange);

    addNode(dosageTimeRange, "start", dateTimeToString(_timeRange->getStartDate()));
    addNode(dosageTimeRange, "end", dateTimeToString(_timeRange->getEndDate()));

    Tucuxi::Common::XmlNode dosage = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dosage");
    dosageTimeRange.addChild(dosage);

    return exportAbstractDosage(*_timeRange->getDosage(), dosage);
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TRY_EXPORT(Type)                                                                    \
    if (dynamic_cast<const Tucuxi::Core::Type*>(&_dosage)) {                                \
        return exportDosage(*dynamic_cast<const Tucuxi::Core::Type*>(&_dosage), _rootNode); \
    }

bool ComputingRequestXmlExport::exportAbstractDosage(
        const Tucuxi::Core::Dosage& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    // The calls order is important here.
    // First start with the subclasses, else it won't work
    TRY_EXPORT(WeeklyDose);
    TRY_EXPORT(DailyDose);
    TRY_EXPORT(LastingDose);
    TRY_EXPORT(ParallelDosageSequence);
    TRY_EXPORT(DosageSteadyState);
    TRY_EXPORT(DosageLoop);
    TRY_EXPORT(DosageSteadyState);
    TRY_EXPORT(DosageRepeat);
    TRY_EXPORT(DosageSequence);

    return false;
}

bool ComputingRequestXmlExport::exportDosage(
        const Tucuxi::Core::DosageBounded& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    return exportAbstractDosage(_dosage, _rootNode);
}

bool ComputingRequestXmlExport::exportDosage(
        const Tucuxi::Core::DosageLoop& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode dosageLoop = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dosageLoop");
    _rootNode.addChild(dosageLoop);

    return exportAbstractDosage(*_dosage.getDosage(), dosageLoop);
}

bool ComputingRequestXmlExport::exportDosage(
        const Tucuxi::Core::DosageSteadyState& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode dosageSteadyState =
            m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dosageSteadyState");
    _rootNode.addChild(dosageSteadyState);

    addNode(dosageSteadyState, "lastDoseDate", dateTimeToString(_dosage.getLastDoseTime()));

    return exportAbstractDosage(*_dosage.getDosage(), dosageSteadyState);
}

bool ComputingRequestXmlExport::exportDosage(
        const Tucuxi::Core::ParallelDosageSequence& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    //TODO : check
    auto it = _dosage.getOffsetsList().begin();

    for (const std::unique_ptr<Tucuxi::Core::DosageBounded>& dosage : _dosage.getDosageList()) {
        addNode(_rootNode, "offsets", timeToString(TimeOfDay(*it)));
        exportAbstractDosage(*dosage, _rootNode);
        it++;
    }

    return true;
}

bool ComputingRequestXmlExport::exportDosage(
        const Tucuxi::Core::DosageRepeat& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode dosageRepeat = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dosageRepeat");
    _rootNode.addChild(dosageRepeat);

    addNode(dosageRepeat, "iterations", _dosage.getNbTimes());

    return exportAbstractDosage(*_dosage.getDosage(), dosageRepeat);
}

bool ComputingRequestXmlExport::exportDosage(
        const Tucuxi::Core::DosageSequence& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode dosageSequence = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dosageSequence");
    _rootNode.addChild(dosageSequence);


    for (const std::unique_ptr<Tucuxi::Core::DosageBounded>& dosage : _dosage.getDosageList()) {
        exportAbstractDosage(*dosage, dosageSequence);
    }

    return true;
}

bool ComputingRequestXmlExport::exportDosage(
        const Tucuxi::Core::LastingDose& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode lastingDose = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "lastingDosage");
    _rootNode.addChild(lastingDose);

    // Be careful here, we use an unnormalized TimeOfDay to allow hours to be more than 23
    addNode(lastingDose, "interval", timeToString(TimeOfDay::buildUnnormalized(_dosage.getTimeStep())));

    exportSingleDose(_dosage, lastingDose);


    return true;
}

bool ComputingRequestXmlExport::exportDosage(const Tucuxi::Core::DailyDose& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode dailyDose = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dailyDosage");
    _rootNode.addChild(dailyDose);

    addNode(dailyDose, "time", timeToString(_dosage.getTimeOfDay()));

    exportSingleDose(_dosage, dailyDose);

    return true;
}

bool ComputingRequestXmlExport::exportDosage(
        const Tucuxi::Core::WeeklyDose& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode weeklyDose = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "weeklyDosage");
    _rootNode.addChild(weeklyDose);

    //TODO : CONTROL
    addNode(weeklyDose, "day", _dosage.getDayOfWeek().c_encoding());

    addNode(weeklyDose, "time", timeToString(_dosage.getTimeOfDay()));

    exportSingleDose(_dosage, weeklyDose);

    return true;
}

void ComputingRequestXmlExport::exportSingleDose(
        const Tucuxi::Core::SingleDose& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    exportDose(_dosage, _rootNode);

    exportFormulationAndRoute(_dosage, _rootNode);
}

void ComputingRequestXmlExport::exportDose(const Tucuxi::Core::SingleDose& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode dose = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dose");
    _rootNode.addChild(dose);

    addNode(dose, "value", static_cast<double>(_dosage.getDose()));
    addNode(dose, "unit", std::string("mg"));
    addNode(dose, "infusionTimeInMinutes", _dosage.getInfusionTime().toMinutes());
}

void ComputingRequestXmlExport::exportFormulationAndRoute(
        const Tucuxi::Core::SingleDose& _dosage, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode formulationAndRoute =
            m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "formulationAndRoute");
    _rootNode.addChild(formulationAndRoute);

    addNode(formulationAndRoute, "formulation", _dosage.getLastFormulationAndRoute().getFormulation());
    addNode(formulationAndRoute, "administrationName", _dosage.getLastFormulationAndRoute().getAdministrationName());
    addNode(formulationAndRoute, "administrationRoute", _dosage.getLastFormulationAndRoute().getAdministrationRoute());
    addNode(formulationAndRoute, "absorptionModel", _dosage.getLastFormulationAndRoute().getAbsorptionModel());
}

std::string ComputingRequestXmlExport::toString(const Tucuxi::Core::Formulation& _formulation)
{
    static std::map<Tucuxi::Core::Formulation, std::string> m = {
            {Tucuxi::Core::Formulation::Undefined, "undefined"},
            {Tucuxi::Core::Formulation::Test, "test"},
            {Tucuxi::Core::Formulation::OralSolution, "oralSolution"},
            {Tucuxi::Core::Formulation::ParenteralSolution, "parenteralSolution"}};

    auto it = m.find(_formulation);
    if (it != m.end()) {
        return it->second;
    }

    return "nothing";
}


std::string ComputingRequestXmlExport::toString(const Tucuxi::Core::AdministrationRoute& _administrationRoute)
{
    static std::map<Tucuxi::Core::AdministrationRoute, std::string> m = {
            {Tucuxi::Core::AdministrationRoute::Oral, "oral"},
            {Tucuxi::Core::AdministrationRoute::Nasal, "nasal"},
            {Tucuxi::Core::AdministrationRoute::Rectal, "rectal"},
            {Tucuxi::Core::AdministrationRoute::Vaginal, "vaginal"},
            {Tucuxi::Core::AdministrationRoute::Undefined, "undefined"},
            {Tucuxi::Core::AdministrationRoute::Sublingual, "sublingual"},
            {Tucuxi::Core::AdministrationRoute::Transdermal, "transdermal"},
            {Tucuxi::Core::AdministrationRoute::Subcutaneous, "subcutaneous"},
            {Tucuxi::Core::AdministrationRoute::Intramuscular, "intramuscular"},
            {Tucuxi::Core::AdministrationRoute::IntravenousDrip, "intravenousDrip"},
            {Tucuxi::Core::AdministrationRoute::IntravenousBolus, "intravenousBolus"}};

    auto it = m.find(_administrationRoute);
    if (it != m.end()) {
        return it->second;
    }

    return "nothing";
}

std::string ComputingRequestXmlExport::toString(const Tucuxi::Core::AbsorptionModel& _absorptionModel)
{
    static std::map<Tucuxi::Core::AbsorptionModel, std::string> m = {
            {Tucuxi::Core::AbsorptionModel::Undefined, "undefined"},
            {Tucuxi::Core::AbsorptionModel::Infusion, "infusion"},
            {Tucuxi::Core::AbsorptionModel::Extravascular, "extravascular"},
            {Tucuxi::Core::AbsorptionModel::Intravascular, "intravascular"},
            {Tucuxi::Core::AbsorptionModel::ExtravascularLag, "extravascularLag"}

    };

    auto it = m.find(_absorptionModel);
    if (it != m.end()) {
        return it->second;
    }

    return "nothing";
}

std::string ComputingRequestXmlExport::toString(DataType _nodeValue)
{
    static const std::map<DataType, std::string> M = {
            {DataType::Bool, "bool"}, {DataType::Date, "date"}, {DataType::Double, "double"}, {DataType::Int, "int"}};
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(PredictionParameterType _nodeValue)
{
    static const std::map<PredictionParameterType, std::string> M = {
            {PredictionParameterType::Population, "population"},
            {PredictionParameterType::Apriori, "apriori"},
            {PredictionParameterType::Aposteriori, "aposteriori"}};
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(TargetType _nodeValue)
{
    static std::map<TargetType, std::string> m = {
            {TargetType::Peak, "peak"},
            {TargetType::Residual, "residual"},
            {TargetType::Mean, "mean"},
            {TargetType::Auc, "auc"},
            {TargetType::Auc24, "auc24"},
            {TargetType::CumulativeAuc, "cumulativeAuc"},
            {TargetType::AucOverMic, "aucOverMic"},
            {TargetType::Auc24OverMic, "auc24OverMic"},
            {TargetType::TimeOverMic, "timeOverMic"},
            {TargetType::AucDividedByMic, "aucDividedByMic"},
            {TargetType::Auc24DividedByMic, "auc24DividedByMic"},
            {TargetType::PeakDividedByMic, "peakDividedByMic"},
            {TargetType::ResidualDividedByMic, "residualDividedByMic"},
            {TargetType::FractionTimeOverMic, "fractionTimeOverMic"}};

    std::string str;
    auto it = m.find(_nodeValue);
    if (it != m.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(CompartmentsOption _nodeValue)
{
    static const std::map<CompartmentsOption, std::string> M = {
            {CompartmentsOption::AllActiveMoieties, "allActiveMoieties"},
            {CompartmentsOption::AllAnalytes, "allAnalytes"},
            {CompartmentsOption::AllCompartments, "allComparements"},
            {CompartmentsOption::MainCompartment, "mainCompartment"},
            {CompartmentsOption::Specific, "specific"}};
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(BestCandidatesOption _nodeValue)
{
    static const std::map<BestCandidatesOption, std::string> M = {
            {BestCandidatesOption::AllDosages, "allDosages"},
            {BestCandidatesOption::BestDosage, "bestDosage"},
            {BestCandidatesOption::BestDosagePerInterval, "bestDosagePerInterval"}};
    std::string str;
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(LoadingOption _nodeValue)
{
    static const std::map<LoadingOption, std::string> M = {
            {LoadingOption::LoadingDoseAllowed, "loadingDoseAllowed"}, {LoadingOption::NoLoadingDose, "noLoadingDose"}};
    std::string str;
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(RestPeriodOption _nodeValue)
{
    static const std::map<RestPeriodOption, std::string> M = {
            {RestPeriodOption::NoRestPeriod, "noRestPeriod"},
            {RestPeriodOption::RestPeriodAllowed, "restPeriodAllowed"}};
    std::string str;
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(SteadyStateTargetOption _nodeValue)
{
    static const std::map<SteadyStateTargetOption, std::string> M = {
            {SteadyStateTargetOption::AtSteadyState, "atSteadyState"},
            {SteadyStateTargetOption::WithinTreatmentTimeRange, "withinTreatmentTimeRange"}};
    std::string str;
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(TargetExtractionOption _nodeValue)
{
    static const std::map<TargetExtractionOption, std::string> M = {
            {TargetExtractionOption::PopulationValues, "populationValues"},
            {TargetExtractionOption::AprioriValues, "aprioriValues"},
            {TargetExtractionOption::IndividualTargets, "individualTargets"},
            {TargetExtractionOption::IndividualTargetsIfDefinitionExists, "individualTargetsIfDefinitionExists"},
            {TargetExtractionOption::DefinitionIfNoIndividualTarget, "definitionIfNoIndividualTarget"},
            {TargetExtractionOption::IndividualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget,
             "individualTargetsIfDefinitionExistsAndDefinitionIfNoIndividualTarget"}};
    std::string str;
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(FormulationAndRouteSelectionOption _nodeValue)
{
    static const std::map<FormulationAndRouteSelectionOption, std::string> M = {
            {FormulationAndRouteSelectionOption::AllFormulationAndRoutes, "allFormulationAndRoutes"},
            {FormulationAndRouteSelectionOption::LastFormulationAndRoute, "lastFormulationAndRoute"},
            {FormulationAndRouteSelectionOption::DefaultFormulationAndRoute, "defaultFormulationAndRoute"}};
    std::string str;
    auto it = M.find(_nodeValue);
    if (it != M.end()) {
        return it->second;
    }
    return "Undefined";
}

std::string ComputingRequestXmlExport::toString(RetrieveStatisticsOption _nodeValue)
{
    return toString(_nodeValue == RetrieveStatisticsOption::RetrieveStatistics);
}

std::string ComputingRequestXmlExport::toString(RetrieveParametersOption _nodeValue)
{
    return toString(_nodeValue == RetrieveParametersOption::RetrieveParameters);
}

std::string ComputingRequestXmlExport::toString(RetrieveCovariatesOption _nodeValue)
{
    return toString(_nodeValue == RetrieveCovariatesOption::RetrieveCovariates);
}

std::string ComputingRequestXmlExport::toString(ForceUgPerLiterOption _nodeValue)
{
    return toString(_nodeValue == ForceUgPerLiterOption::Force);
}

std::string ComputingRequestXmlExport::toString(bool _nodeValue)
{
    if (_nodeValue) {
        return "true";
    }
    return "false";
}




bool ComputingRequestXmlExport::exportDosageHistory(const DosageHistory& _history, Tucuxi::Common::XmlNode& _rootNode)
{
    Tucuxi::Common::XmlNode dosageHistory = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dosageHistory");
    _rootNode.addChild(dosageHistory);

    for (const auto& dosageTimeRange : _history.getDosageTimeRanges()) {
        if (!exportDosageTimeRange(std::move(dosageTimeRange), dosageHistory)) {
            // TODO : Error

            return false;
        }
    }

    //    TMP_UNUSED_PARAMETER(_history);
    //    TMP_UNUSED_PARAMETER(_rootNode);

    return true;
}

} // namespace Core
} // namespace Tucuxi
