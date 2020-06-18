#include "computingqueryresponsexmlexport.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <mutex>

#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/general.h"


namespace Tucuxi {
namespace Query {

using namespace Core;

ComputingQueryResponseXmlExport::ComputingQueryResponseXmlExport()
{}

bool ComputingQueryResponseXmlExport::exportToFile(const ComputingQueryResponse &_computingQueryResponse, std::string _fileName)
{
    std::string xmlString;
    if (!exportToString(_computingQueryResponse, xmlString)) {
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


bool ComputingQueryResponseXmlExport::exportToString(const ComputingQueryResponse &_computingQueryResponse, std::string &_xmlString)
{
    static const bool STATUS_CODE_NUMBER = true;
    static const bool STATUS_CODE_MESSAGE = false;

    // Ensure the function is reentrant
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    Tucuxi::Common::XmlNode root = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "tucuxiComputation");
    auto attribute1 = m_doc.createAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    auto attribute2 = m_doc.createAttribute("xsi:noNamespaceSchemaLocation", "computing_response.xsd");
    root.addAttribute(attribute1);
    root.addAttribute(attribute2);
    m_doc.setRoot(root);
    Tucuxi::Common::XmlNode queryId = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element,
                                                       "queryId", _computingQueryResponse.getQueryId());
    root.addChild(queryId);

    Tucuxi::Common::XmlNode queryStatus = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element,
                                                           "queryStatus", "queryStatus");
    root.addChild(queryStatus);

    Tucuxi::Common::XmlNode statusCode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element,
                                                          "statusCode", getQueryStatus(_computingQueryResponse.getQueryStatus(), STATUS_CODE_NUMBER));
    queryStatus.addChild(statusCode);

    Tucuxi::Common::XmlNode statusCodeLit = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element,
                                                             "statusCodeLit", getQueryStatus(_computingQueryResponse.getQueryStatus(), STATUS_CODE_MESSAGE));
    queryStatus.addChild(statusCodeLit);

    Tucuxi::Common::XmlNode message = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element,
                                                       "message", _computingQueryResponse.getErrorMessage());
    queryStatus.addChild(message);

    Tucuxi::Common::XmlNode description = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "description");
    queryStatus.addChild(description);

    if((_computingQueryResponse.getQueryStatus() == QueryStatus::ImportError)
            || (_computingQueryResponse.getQueryStatus() == QueryStatus::BadFormat))
    {
        //Only queryId and queryStatus Information
        m_doc.toString(_xmlString, true);
        return true;
    }

    Tucuxi::Common::XmlNode responses = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "responses");
    root.addChild(responses);



    for (const auto &response : _computingQueryResponse.getRequestResponses()) {


        Tucuxi::Common::XmlNode responseNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "response");

        responses.addChild(responseNode);

        Tucuxi::Common::XmlNode requestId = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "requestId",
                                                         response.m_computingResponse->getId());
        responseNode.addChild(requestId);

        Tucuxi::Common::XmlNode requestStatus = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "requestStatus");
        responseNode.addChild(requestStatus);

        Tucuxi::Common::XmlNode statusCode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "statusCode", getComputingStatus(response.m_computingResponse->getComputingStatus(), STATUS_CODE_NUMBER));
        requestStatus.addChild(statusCode);

        Tucuxi::Common::XmlNode statusCodeLit = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "statusCodeLit", getComputingStatus(response.m_computingResponse->getComputingStatus(), STATUS_CODE_MESSAGE));
        requestStatus.addChild(statusCodeLit);

        Tucuxi::Common::XmlNode message = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "message");
        requestStatus.addChild(message);

        Tucuxi::Common::XmlNode description = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "description");
        requestStatus.addChild(description);


        // We start by checking for adjustements, as AdjustmentResponse is a subclass of SinglePredictionResponse
        if (dynamic_cast<const Tucuxi::Core::AdjustmentData*>(response.m_computingResponse->getData()) != nullptr) {

            Tucuxi::Common::XmlNode dataNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dataAdjustment");

            Tucuxi::Common::XmlNode requestType = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "requestType", "adjustment");
            responseNode.addChild(requestType);

            const Tucuxi::Core::AdjustmentData* prediction =
                    dynamic_cast<const Tucuxi::Core::AdjustmentData*>(response.m_computingResponse->getData());

            if (!exportAdjustment(prediction, dataNode)) {
                //Error
            }
            responseNode.addChild(dataNode);

        }
        else if (dynamic_cast<const Tucuxi::Core::SinglePredictionData*>(response.m_computingResponse->getData()) != nullptr) {

            Tucuxi::Common::XmlNode dataNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dataPrediction");

            Tucuxi::Common::XmlNode requestType = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "requestType", "prediction");
            responseNode.addChild(requestType);

            const Tucuxi::Core::SinglePredictionData* prediction =
                    dynamic_cast<const Tucuxi::Core::SinglePredictionData*>(response.m_computingResponse->getData());

            if (!exportSinglePrediction(prediction, dataNode)) {
                //Error
            }
            responseNode.addChild(dataNode);

        }
        else if (dynamic_cast<const Tucuxi::Core::SinglePointsData*>(response.m_computingResponse->getData()) != nullptr) {

            Tucuxi::Common::XmlNode dataNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dataPoints");

            Tucuxi::Common::XmlNode requestType = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "requestType", "singlePoints");
            responseNode.addChild(requestType);

            const Tucuxi::Core::SinglePointsData* prediction =
                    dynamic_cast<const Tucuxi::Core::SinglePointsData*>(response.m_computingResponse->getData());

            if (!exportSinglePoints(prediction, dataNode)) {
                //Error
            }
            responseNode.addChild(dataNode);

        }
        else if (dynamic_cast<const Tucuxi::Core::PercentilesData*>(response.m_computingResponse->getData()) != nullptr) {

            Tucuxi::Common::XmlNode dataNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "dataPercentiles");

            Tucuxi::Common::XmlNode requestType = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "requestType", "percentiles");
            responseNode.addChild(requestType);

            const Tucuxi::Core::PercentilesData* prediction =
                    dynamic_cast<const Tucuxi::Core::PercentilesData*>(response.m_computingResponse->getData());

            if (!exportPercentiles(prediction, dataNode)) {
                //Error
            }
            responseNode.addChild(dataNode);

        }
        else {
            // TODO : Not supported export
        }
    }

    m_doc.toString(_xmlString, true);
    return true;
}

const std::string ComputingQueryResponseXmlExport::getComputingStatus(Tucuxi::Core::ComputingStatus _computingStatus, bool _codeEnable) const
{
    static std::map<Tucuxi::Core::ComputingStatus, std::pair<std::string, std::string>> m =
    {
        {Tucuxi::Core::ComputingStatus::Ok, {"Ok", "0"}},
        {Tucuxi::Core::ComputingStatus::Undefined, {"Undefined", "-1"}},
        {Tucuxi::Core::ComputingStatus::TooBig, {"TooBig", "1"}},
        {Tucuxi::Core::ComputingStatus::Aborted, {"Aborted", "2"}},
        {Tucuxi::Core::ComputingStatus::ParameterExtractionError, {"ParameterExtractionError", "3"}},
        {Tucuxi::Core::ComputingStatus::SampleExtractionError, {"SampleExtractionError", "4"}},
        {Tucuxi::Core::ComputingStatus::TargetExtractionError, {"TargetExtractionError", "5"}},
        {Tucuxi::Core::ComputingStatus::InvalidCandidate, {"InvalidCandidate", "6"}},
        {Tucuxi::Core::ComputingStatus::TargetEvaluationError, {"TargetEvaluationError", "7"}},
        {Tucuxi::Core::ComputingStatus::CovariateExtractionError, {"CovariateExtractionError", "8"}},
        {Tucuxi::Core::ComputingStatus::IntakeExtractionError, {"IntakeExtractionError", "9"}},
        {Tucuxi::Core::ComputingStatus::ErrorModelExtractionError, {"ErrorModelExtractionError", "10"}},
        {Tucuxi::Core::ComputingStatus::UnsupportedRoute, {"UnsupportedRoute", "11"}},
        {Tucuxi::Core::ComputingStatus::AnalyteConversionError, {"AnalyteConversionError", "12"}},
        {Tucuxi::Core::ComputingStatus::AposterioriPercentilesNoSamplesError, {"AposterioriPercentilesNoSamplesError", "13"}},
        {Tucuxi::Core::ComputingStatus::ConcentrationCalculatorNoParameters, {"ConcentrationCalculatorNoParameters", "14"}},
        {Tucuxi::Core::ComputingStatus::BadParameters, {"BadParameters", "15"}},
        {Tucuxi::Core::ComputingStatus::BadConcentration, {"BadConcentration", "16"}},
        {Tucuxi::Core::ComputingStatus::DensityError, {"DensityError", "17"}},
        {Tucuxi::Core::ComputingStatus::AposterioriEtasCalculationEmptyOmega, {"AposterioriEtasCalculationEmptyOmega", "18"}},
        {Tucuxi::Core::ComputingStatus::AposterioriEtasCalculationNoSquareOmega, {"AposterioriEtasCalculationNoSquareOmega", "19"}},
        {Tucuxi::Core::ComputingStatus::ComputingTraitStandardShouldNotBeCalled, {"ComputingTraitStandardShouldNotBeCalled", "20"}},
        {Tucuxi::Core::ComputingStatus::CouldNotFindSuitableFormulationAndRoute, {"CouldNotFindSuitableFormulationAndRoute", "21"}},
        {Tucuxi::Core::ComputingStatus::MultipleFormulationAndRoutesNotSupported, {"MultipleFormulationAndRoutesNotSupported", "22"}},
        {Tucuxi::Core::ComputingStatus::NoPkModelError, {"NoPkModelError", "23"}},
        {Tucuxi::Core::ComputingStatus::ComputingComponentExceptionError, {"ComputingComponentExceptionError", "24"}},
        {Tucuxi::Core::ComputingStatus::NoPkModels, {"NoPkModels", "25"}},
        {Tucuxi::Core::ComputingStatus::NoComputingTraits, {"NoComputingTraits", "26"}},
        {Tucuxi::Core::ComputingStatus::RecordedIntakesSizeError, {"RecordedIntakesSizeError", "27"}},
        {Tucuxi::Core::ComputingStatus::NoPercentilesCalculation, {"NoPercentilesCalculation", "28"}},
        {Tucuxi::Core::ComputingStatus::SelectedIntakesSizeError, {"SelectedIntakesSizeError", "29"}},
        {Tucuxi::Core::ComputingStatus::NoAvailableDose, {"NoAvailableDose", "30"}},
        {Tucuxi::Core::ComputingStatus::NoAvailableInterval, {"NoAvailableInterval", "31"}},
        {Tucuxi::Core::ComputingStatus::NoAvailableInfusionTime, {"NoAvailableInfusionTime", "32"}},
        {Tucuxi::Core::ComputingStatus::NoFormulationAndRouteForAdjustment, {"NoFormulationAndRouteForAdjustment", "33"}},
        {Tucuxi::Core::ComputingStatus::ConcentrationSizeError, {"ConcentrationSizeError", "34"}},
        {Tucuxi::Core::ComputingStatus::ActiveMoietyCalculationError, {"ActiveMoietyCalculationError", "35"}},
        {Tucuxi::Core::ComputingStatus::NoAnalytesGroup, {"NoAnalytesGroup", "36"}},
        {Tucuxi::Core::ComputingStatus::IncompatibleTreatmentModel, {"IncompatibleTreatmentModel", "37"}},
        {Tucuxi::Core::ComputingStatus::ComputingComponentNotInitialized, {"ComputingComponentNotInitialized", "38"}},
        {Tucuxi::Core::ComputingStatus::UncompatibleDrugDomain, {"UncompatibleDrugDomain", "39"}}
    };

    auto it = m.find(_computingStatus);
    if (it != m.end()) {
        std::pair<std::string, std::string> m2 = it->second;
        if(_codeEnable)
        {
            return m2.second;
        }
        else{
            return m2.first;
        }
    }

    return "nothing";
}

const std::string ComputingQueryResponseXmlExport::getQueryStatus(QueryStatus _queryStatus, bool _codeEnable) const
{
    static std::map<QueryStatus, std::pair<std::string, std::string>> m =
    {
        {QueryStatus::Ok, {"Ok", "0"}},
        {QueryStatus::PartiallyOk, {"PartiallyOk", "1"}},
        {QueryStatus::Error, {"Error", "2"}},
        {QueryStatus::ImportError, {"ImportError", "3"}},
        {QueryStatus::BadFormat, {"BadFormat", "4"}},
        {QueryStatus::Undefined, {"Undefined", "5"}}

    };

    auto pair = m.find(_queryStatus);
    if (pair != m.end()) {
        std::pair<std::string, std::string> m2 = pair->second;
        if(_codeEnable)
        {
            return m2.second;
        }
        else{
            return m2.first;
        }
    }

    return "nothing";
}

bool ComputingQueryResponseXmlExport::exportAdjustment(const Tucuxi::Core::AdjustmentData *_prediction,
                      Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode analyteIds = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "analyteIds");
    _rootNode.addChild(analyteIds);
    for (const auto &analyteId: _prediction->getIds()) {
        addNode(analyteIds, "analyteId", analyteId);
    }

    Tucuxi::Common::XmlNode adjustments = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "adjustments");
    _rootNode.addChild(adjustments);

    for(const auto &adj : _prediction->getAdjustments()) {
        Tucuxi::Common::XmlNode adjustment = m_doc.createNode(
                    Tucuxi::Common::EXmlNodeType::Element, "adjustment");
        adjustments.addChild(adjustment);
        addNode(adjustment, "score", adj.getGlobalScore());
        Tucuxi::Common::XmlNode targetEvaluations = m_doc.createNode(
                    Tucuxi::Common::EXmlNodeType::Element, "targetEvaluations");
        adjustment.addChild(targetEvaluations);

        for (const auto &target : adj.m_targetsEvaluation) {
            Tucuxi::Common::XmlNode targetEvaluation = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "targetEvaluation");
            targetEvaluations.addChild(targetEvaluation);
            addNode(targetEvaluation, "targetType", toString(target.getTargetType()));
            addNode(targetEvaluation, "unit", target.getUnit().toString());
            addNode(targetEvaluation, "value", target.getValue());
            addNode(targetEvaluation, "score", target.getScore());
        }
        if (!exportDosageHistory(adj.m_history, adjustment)) {
            return false;
        }
        if (!exportCycleDatas(adj.m_data, adjustment)) {
            return false;
        }

    }

    return true;
}

bool ComputingQueryResponseXmlExport::exportSinglePrediction(const Tucuxi::Core::SinglePredictionData *_prediction,
                      Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode analyteIds = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "analyteIds");
    _rootNode.addChild(analyteIds);
    for (const auto &analyteId: _prediction->getIds()) {
        addNode(analyteIds, "analyteId", analyteId);
    }

    if (!exportCycleDatas(_prediction->getData(), _rootNode)) {

        return false;
    }

    return true;
}

std::string dateTimeToString(const Tucuxi::Common::DateTime &_dateTime)
{
    if (_dateTime.isUndefined()) {
        return "";
    }
    std::string result;
    result = std::to_string(_dateTime.year())
            + "." + std::to_string(_dateTime.month()) +
            "." + std::to_string(_dateTime.day()) +
            "T" + std::to_string(_dateTime.hour())
            + ":" + std::to_string(_dateTime.minute()) + ":"
            + std::to_string(_dateTime.second());

    char str[20];
    snprintf (str, 20, "%04d-%02d-%02dT%02d:%02d:%02d", _dateTime.year(),
              _dateTime.month(), _dateTime.day(), _dateTime.hour(), _dateTime.minute(), _dateTime.second());
    result = str;
    return result;
}

std::string timeToString(const Tucuxi::Common::TimeOfDay &_timeOfDay)
{
    char str[10];
    snprintf (str, 9, "%02d:%02d:%02d", _timeOfDay.hour(), _timeOfDay.minute(), _timeOfDay.second());

    return std::string(str);
}



bool ComputingQueryResponseXmlExport::exportSinglePoints(const Tucuxi::Core::SinglePointsData *_prediction,
                      Tucuxi::Common::XmlNode &_rootNode)
{
    addNode(_rootNode, "unit", _prediction->m_unit.toString());

    Tucuxi::Common::XmlNode points = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "points");
    _rootNode.addChild(points);

    for(size_t i = 0; i < _prediction->m_times.size(); i++) {
        Tucuxi::Common::XmlNode point = m_doc.createNode(
                    Tucuxi::Common::EXmlNodeType::Element, "point");
        points.addChild(point);

        addNode(point, "time", dateTimeToString(_prediction->m_times[i]));
        addNode(point, "value", std::to_string(_prediction->m_concentrations[0][i]));
    }

    return true;
}

void ComputingQueryResponseXmlExport::addNode(Tucuxi::Common::XmlNode &_rootNode,
             std::string _nodeName,
             std::string _nodeValue)
{
    Tucuxi::Common::XmlNode node = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, _nodeName, _nodeValue);
    _rootNode.addChild(node);
}

bool ComputingQueryResponseXmlExport::exportPercentiles(const Tucuxi::Core::PercentilesData *_prediction,
                      Tucuxi::Common::XmlNode &_rootNode)
{
    size_t nbRanks = _prediction->getNbRanks();
    for (unsigned int index = 0; index < nbRanks; index++) {
        Tucuxi::Common::XmlNode percentile = m_doc.createNode(
                    Tucuxi::Common::EXmlNodeType::Element, "percentile");
        _rootNode.addChild(percentile);
        addNode(percentile, "rank", std::to_string(_prediction->getRank(index)));
        if (!exportCycleDatas(_prediction->getPercentileData(index), percentile)) {
            // TODO : Manage that
            return false;
        }
    }
    return true;
}



bool ComputingQueryResponseXmlExport::exportDosageTimeRange(const std::unique_ptr<Tucuxi::Core::DosageTimeRange> &_timeRange,
                                                            Tucuxi::Common::XmlNode &_rootNode)
{

    Tucuxi::Common::XmlNode dosageTimeRange = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dosageTimeRange");
    _rootNode.addChild(dosageTimeRange);

    addNode(dosageTimeRange, "start", dateTimeToString(_timeRange->getStartDate()));
    addNode(dosageTimeRange, "end", dateTimeToString(_timeRange->getEndDate()));

    Tucuxi::Common::XmlNode dosage = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dosage");
    dosageTimeRange.addChild(dosage);

    return exportAbstractDosage(*_timeRange->getDosage(), dosage);
}

#define TRY_EXPORT(Type) \
if (dynamic_cast<const Tucuxi::Core::Type*>(&_dosage)) { \
    return exportDosage(*dynamic_cast<const Tucuxi::Core::Type*>(&_dosage), _rootNode); \
}

bool ComputingQueryResponseXmlExport::exportAbstractDosage(const Tucuxi::Core::Dosage &_dosage, Tucuxi::Common::XmlNode &_rootNode)
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

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::DosageBounded &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    return exportAbstractDosage(_dosage, _rootNode);
}

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::DosageLoop &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode dosageLoop = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dosageLoop");
    _rootNode.addChild(dosageLoop);

    return exportAbstractDosage(*_dosage.getDosage(), dosageLoop);
}

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::DosageSteadyState &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode dosageSteadyState = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dosageSteadyState");
    _rootNode.addChild(dosageSteadyState);

    addNode(dosageSteadyState, "lastDoseDate", dateTimeToString(_dosage.getLastDoseTime()));

    return exportAbstractDosage(*_dosage.getDosage(), dosageSteadyState);
}

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::ParallelDosageSequence &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    //TODO : check
    auto it = _dosage.getOffsetsList().begin();

    for(const std::unique_ptr<Tucuxi::Core::DosageBounded> &dosage : _dosage.getDosageList())
    {
        addNode(_rootNode, "offsets", timeToString(TimeOfDay(*it)));
        exportAbstractDosage(*dosage, _rootNode);
        it++;
    }

    return true;
}

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::DosageRepeat &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode dosageRepeat = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dosageRepeat");
    _rootNode.addChild(dosageRepeat);

    addNode(dosageRepeat, "iterations", _dosage.getNbTimes());

    return exportAbstractDosage(*_dosage.getDosage(), dosageRepeat);
}

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::DosageSequence &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode dosageSequence = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dosageSequence");
    _rootNode.addChild(dosageSequence);


    for(const std::unique_ptr<Tucuxi::Core::DosageBounded> &dosage : _dosage.getDosageList())
    {
        exportAbstractDosage(*dosage, dosageSequence);
    }

    return true;
}

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::LastingDose &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode lastingDose = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "lastingDosage");
    _rootNode.addChild(lastingDose);

    addNode(lastingDose, "interval", timeToString(TimeOfDay(_dosage.getTimeStep())));

    exportSingleDose(_dosage, lastingDose);


    return true;
}

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::DailyDose &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode dailyDose = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dailyDosage");
    _rootNode.addChild(dailyDose);

    addNode(dailyDose, "time", timeToString(_dosage.getTimeOfDay()));

    exportSingleDose(_dosage, dailyDose);

    return true;
}

bool ComputingQueryResponseXmlExport::exportDosage(const Tucuxi::Core::WeeklyDose &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode weeklyDose = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "weeklyDosage");
    _rootNode.addChild(weeklyDose);

    //TODO : CONTROL
    addNode(weeklyDose, "day", _dosage.getDayOfWeek().operator unsigned int());

    addNode(weeklyDose, "time", timeToString(_dosage.getTimeOfDay()));

    exportSingleDose(_dosage, weeklyDose);

    return true;
}

void ComputingQueryResponseXmlExport::exportSingleDose(const Tucuxi::Core::SingleDose &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    exportDose(_dosage, _rootNode);

    exportFormulationAndRoute(_dosage, _rootNode);

}

void ComputingQueryResponseXmlExport::exportDose(const Tucuxi::Core::SingleDose &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode dose = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dose");
    _rootNode.addChild(dose);

    addNode(dose, "value", double(_dosage.getDose()));
    addNode(dose, "unit", std::string("mg"));
    addNode(dose, "infusionTimeInMinutes", _dosage.getInfusionTime().toMinutes());

}

void ComputingQueryResponseXmlExport::exportFormulationAndRoute(const Tucuxi::Core::SingleDose &_dosage, Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode formulationAndRoute = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "formulationAndRoute");
    _rootNode.addChild(formulationAndRoute);

    addNode(formulationAndRoute, "formulation", formulationEnumToString(_dosage.getLastFormulationAndRoute().getFormulation()));
    addNode(formulationAndRoute, "administrationName", _dosage.getLastFormulationAndRoute().getAdministrationName());
    addNode(formulationAndRoute, "administrationRoute", administrationRouteEnumToString(_dosage.getLastFormulationAndRoute().getAdministrationRoute()));
    addNode(formulationAndRoute, "absorptionModel", absorptionModelEnumToString(_dosage.getLastFormulationAndRoute().getAbsorptionModel()));


}

const std::string ComputingQueryResponseXmlExport::formulationEnumToString(const Tucuxi::Core::Formulation &_formulation)
{
    static std::map<Tucuxi::Core::Formulation, std::string> m =
    {
        {Tucuxi::Core::Formulation::Undefined, "undefined"},
        {Tucuxi::Core::Formulation::Test, "test"},
        {Tucuxi::Core::Formulation::OralSolution, "oralSolution"},
        {Tucuxi::Core::Formulation::ParenteralSolution, "parenteralSolution"}
    };

    auto it = m.find(_formulation);
    if (it != m.end()) {
        return it->second;
    }

    return "nothing";

}


const std::string ComputingQueryResponseXmlExport::administrationRouteEnumToString(const Tucuxi::Core::AdministrationRoute &_administrationRoute)
{
    static std::map<Tucuxi::Core::AdministrationRoute, std::string> m =
    {
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
        {Tucuxi::Core::AdministrationRoute::IntravenousBolus, "intravenousBolus"}
    };

    auto it = m.find(_administrationRoute);
    if (it != m.end()) {
        return it->second;
    }

    return "nothing";
}

const std::string ComputingQueryResponseXmlExport::absorptionModelEnumToString(const Tucuxi::Core::AbsorptionModel &_absorptionModel)
{
    static std::map<Tucuxi::Core::AbsorptionModel, std::string> m =
    {
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




bool ComputingQueryResponseXmlExport::exportCycleData(const Tucuxi::Core::CycleData &_cycleData,
        Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode cycleData = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "cycleData");
    _rootNode.addChild(cycleData);

    addNode(cycleData, "start", dateTimeToString(_cycleData.m_start));
    addNode(cycleData, "end", dateTimeToString(_cycleData.m_end));
    addNode(cycleData, "unit", _cycleData.m_unit.toString());

    Tucuxi::Common::XmlNode parameters = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "parameters");
    cycleData.addChild(parameters);
    for(const auto &parameter : _cycleData.m_parameters) {
        Tucuxi::Common::XmlNode parameterNode = m_doc.createNode(
                    Tucuxi::Common::EXmlNodeType::Element, "parameter");
        parameters.addChild(parameterNode);
        addNode(parameterNode, "id", parameter.m_parameterId);
        addNode(parameterNode, "value", std::to_string(parameter.m_value));
    }


    Tucuxi::Common::XmlNode covariates = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "covariates");
    cycleData.addChild(covariates);
    for(const auto &covariate : _cycleData.m_covariates) {
        Tucuxi::Common::XmlNode covariateNode = m_doc.createNode(
                    Tucuxi::Common::EXmlNodeType::Element, "covariate");
        covariates.addChild(covariateNode);
        addNode(covariateNode, "id", covariate.m_covariateId);
        addNode(covariateNode, "value", std::to_string(covariate.m_value));
    }

    // Concatenate the times
    std::string timesString;
    for(size_t i = 0; i < _cycleData.m_times[0].size(); i++) {
        timesString += std::to_string(_cycleData.m_times[0][i]);
        if (i != _cycleData.m_times[0].size() - 1) {
            timesString += ',';
        }
    }
    addNode(cycleData, "times", timesString);

    // Concatenate the points
    std::string pointsString;
    for(size_t i = 0; i < _cycleData.m_concentrations[0].size(); i++) {
        pointsString += std::to_string(_cycleData.m_concentrations[0][i]);
        if (i != _cycleData.m_concentrations[0].size() - 1) {
            pointsString += ',';
        }
    }
    addNode(cycleData, "values", pointsString);

    if(!_cycleData.m_statistics.getStats().empty())
    {

        Tucuxi::Common::XmlNode statistics = m_doc.createNode(
                    Tucuxi::Common::EXmlNodeType::Element, "statistics");
        cycleData.addChild(statistics);

        // Extract statistics

        double mean = -1.0;
        double auc = -1.0;
        double auc24 = -1.0;
        double cumulativeAuc = -1.0;
        double residual = -1.0;
        double peak = -1.0;
    //    double minimum = -1.0;
    //    double maximum = -1.0;

        Tucuxi::Common::DateTime date;
        bool ok = true;
        ok &= _cycleData.m_statistics.getStatistic(0, Tucuxi::Core::CycleStatisticType::Mean).getValue(date, mean);
        ok &= _cycleData.m_statistics.getStatistic(0, Tucuxi::Core::CycleStatisticType::AUC).getValue(date, auc);
        ok &= _cycleData.m_statistics.getStatistic(0, Tucuxi::Core::CycleStatisticType::AUC24).getValue(date, auc24);
        ok &= _cycleData.m_statistics.getStatistic(0, Tucuxi::Core::CycleStatisticType::CumulativeAuc).getValue(date, cumulativeAuc);
        ok &= _cycleData.m_statistics.getStatistic(0, Tucuxi::Core::CycleStatisticType::Residual).getValue(date, residual);
        ok &= _cycleData.m_statistics.getStatistic(0, Tucuxi::Core::CycleStatisticType::Peak).getValue(date, peak);
        //ok &= _cycleData.m_statistics.getStatistic(0, Tucuxi::Core::CycleStatisticType::Minimum).getValue(date, minimum);
        //ok &= _cycleData.m_statistics.getStatistic(0, Tucuxi::Core::CycleStatisticType::Maximum).getValue(date, maximum);

        addNode(statistics, "mean", mean);
        addNode(statistics, "auc", auc);
        addNode(statistics, "auc24", auc24);
        addNode(statistics, "cumulativeAuc", cumulativeAuc);
        addNode(statistics, "residual", residual);
        addNode(statistics, "peak", peak);
        //addNode(statistics, "minimum", minimum);
        //addNode(statistics, "maximum", maximum);
        if (!ok) {
            std::cout << "Something went wrong here" << std::endl;
        }

    }


    return true;
}

bool ComputingQueryResponseXmlExport::exportCycleDatas(const std::vector<Tucuxi::Core::CycleData> &_cycleDatas,
        Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode cycleDatas = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "cycleDatas");
    _rootNode.addChild(cycleDatas);

    for(const auto &cycleData : _cycleDatas) {
        if (!exportCycleData(cycleData, cycleDatas)) {
            // TODO : Error

            return false;
        }
    }
    return true;
}

bool ComputingQueryResponseXmlExport::exportDosageHistory(const DosageHistory &_history,
        Tucuxi::Common::XmlNode &_rootNode)
{
    Tucuxi::Common::XmlNode dosageHistory = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, "dosageHistory");
    _rootNode.addChild(dosageHistory);

    for(const auto &dosageTimeRange : _history.getDosageTimeRanges())
    {
        if (!exportDosageTimeRange(std::move(dosageTimeRange), dosageHistory)) {
            // TODO : Error

            return false;
        }
    }

//    TMP_UNUSED_PARAMETER(_history);
//    TMP_UNUSED_PARAMETER(_rootNode);

    return true;
}

} // namespace Query
} // namespace Tucuxi

