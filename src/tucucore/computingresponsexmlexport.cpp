#include "computingresponsexmlexport.h"

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
namespace Core {

ComputingResponseXmlExport::ComputingResponseXmlExport()
{

}

bool ComputingResponseXmlExport::exportToFile(const ComputingResponse &_computingResponse, std::string _fileName)
{
    std::string xmlString;
    if (!exportToString(_computingResponse, xmlString)) {
        return false;
    }

    std::ofstream file;
    file.open(_fileName);
    if (file.rdstate() & std::ostream::failbit) {
        Tucuxi::Common::LoggerHelper logHelper;
        logHelper.error("The file {} could not be opened.", _fileName);
        return false;
    }
    file << xmlString;
    file.close();

    return true;
}

bool ComputingResponseXmlExport::exportToString(const ComputingResponse &_computingResponse, std::string &_xmlString)
{
    // Ensure the function is reentrant
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    Tucuxi::Common::XmlNode root = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "tucuxiComputation");
    m_doc.setRoot(root);
    Tucuxi::Common::XmlNode queryId = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "queryId", _computingResponse.getId());
    root.addChild(queryId);
    Tucuxi::Common::XmlNode responses = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "responses");
    root.addChild(responses);

/*    Tucuxi::Common::XmlNode node1 = doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "CHILD", "Salut");
    node1.addAttribute(doc.createAttribute("text", "abc"));
    node1.addAttribute(doc.createAttribute("id", "1"));
    root.addChild(node1);
    Tucuxi::Common::XmlNode node2 = doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "CHILD", "Tcho");
    node2.addAttribute(doc.createAttribute("text", "def"));
    node2.addAttribute(doc.createAttribute("id", "2"));
    root.addChild(node2);
*/

    for (auto &response : _computingResponse.getResponses()) {

        Tucuxi::Common::XmlNode responseNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "response");

        responses.addChild(responseNode);

        Tucuxi::Common::XmlNode dataNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "data");

        Tucuxi::Common::XmlNode requestId = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "requestId",
                                                         response->getId());
        responseNode.addChild(requestId);

        Tucuxi::Common::XmlNode issuesNode = m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, "issues");
        responseNode.addChild(issuesNode);

        // We start by checking for adjustements, as AdjustmentResponse is a subclass of SinglePredictionResponse
        if (dynamic_cast<Tucuxi::Core::AdjustmentResponse*>(response.get()) != nullptr) {

            Tucuxi::Common::XmlNode requestType = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "requestType", "adjustment");
            responseNode.addChild(requestType);

            const Tucuxi::Core::AdjustmentResponse* prediction =
                    dynamic_cast<Tucuxi::Core::AdjustmentResponse*>(response.get());

            if (!exportAdjustment(prediction, dataNode)) {
                Tucuxi::Common::XmlNode issue = m_doc.createNode(
                            Tucuxi::Common::EXmlNodeType::Element, "error", "Cannot export the adjustments");
                issuesNode.addChild(issue);
            }

        }
        else if (dynamic_cast<Tucuxi::Core::SinglePredictionResponse*>(response.get()) != nullptr) {

            Tucuxi::Common::XmlNode requestType = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "requestType", "prediction");
            responseNode.addChild(requestType);

            const Tucuxi::Core::SinglePredictionResponse* prediction =
                    dynamic_cast<Tucuxi::Core::SinglePredictionResponse*>(response.get());

            if (!exportSinglePrediction(prediction, dataNode)) {
                Tucuxi::Common::XmlNode issue = m_doc.createNode(
                            Tucuxi::Common::EXmlNodeType::Element, "error", "Cannot export the prediction");
                issuesNode.addChild(issue);
            }

        }
        else if (dynamic_cast<Tucuxi::Core::SinglePointsResponse*>(response.get()) != nullptr) {

            Tucuxi::Common::XmlNode requestType = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "requestType", "singlePoints");
            responseNode.addChild(requestType);

            const Tucuxi::Core::SinglePointsResponse* prediction =
                    dynamic_cast<Tucuxi::Core::SinglePointsResponse*>(response.get());

            if (!exportSinglePoints(prediction, dataNode)) {
                Tucuxi::Common::XmlNode issue = m_doc.createNode(
                            Tucuxi::Common::EXmlNodeType::Element, "error", "Cannot export the points");
                issuesNode.addChild(issue);
            }

        }
        else if (dynamic_cast<Tucuxi::Core::PercentilesResponse*>(response.get()) != nullptr) {

            Tucuxi::Common::XmlNode requestType = m_doc.createNode(
                        Tucuxi::Common::EXmlNodeType::Element, "requestType", "percentiles");
            responseNode.addChild(requestType);

            const Tucuxi::Core::PercentilesResponse* prediction =
                    dynamic_cast<Tucuxi::Core::PercentilesResponse*>(response.get());

            if (!exportPercentiles(prediction, dataNode)) {
                Tucuxi::Common::XmlNode issue = m_doc.createNode(
                            Tucuxi::Common::EXmlNodeType::Element, "error", "Cannot export the percentiles");
                issuesNode.addChild(issue);
            }

        }
        else {
            // TODO : Not supported export
        }

        responseNode.addChild(dataNode);


    }




    m_doc.toString(_xmlString, true);
    return true;
}



bool ComputingResponseXmlExport::exportAdjustment(const Tucuxi::Core::AdjustmentResponse *_prediction,
                      Tucuxi::Common::XmlNode &_rootNode)
{
    TMP_UNUSED_PARAMETER(_prediction);
    TMP_UNUSED_PARAMETER(_rootNode);
    return false;
}

bool ComputingResponseXmlExport::exportSinglePrediction(const Tucuxi::Core::SinglePredictionResponse *_prediction,
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

bool ComputingResponseXmlExport::exportSinglePoints(const Tucuxi::Core::SinglePointsResponse *_prediction,
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

void ComputingResponseXmlExport::addNode(Tucuxi::Common::XmlNode &_rootNode,
             std::string _nodeName,
             std::string _nodeValue)
{
    Tucuxi::Common::XmlNode node = m_doc.createNode(
                Tucuxi::Common::EXmlNodeType::Element, _nodeName, _nodeValue);
    _rootNode.addChild(node);
}

bool ComputingResponseXmlExport::exportPercentiles(const Tucuxi::Core::PercentilesResponse *_prediction,
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
        }
    }
    return false;
}


bool ComputingResponseXmlExport::exportCycleData(const Tucuxi::Core::CycleData &_cycleData,
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


    return true;
}

bool ComputingResponseXmlExport::exportCycleDatas(const std::vector<Tucuxi::Core::CycleData> &_cycleDatas,
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

} // namespace Core
} // namespace Tucuxi

