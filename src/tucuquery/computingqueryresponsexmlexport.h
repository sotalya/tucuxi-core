#ifndef COMPUTINGRESPONSEXMLEXPORT_H
#define COMPUTINGRESPONSEXMLEXPORT_H

#include "tucucore/computingservice/computingresponse.h"

#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"

#include "tucuquery/computingqueryresponse.h"

namespace Tucuxi {
namespace Query {

class ComputingQueryResponseXmlExport
{
public:
    ComputingQueryResponseXmlExport();

    bool exportToString(const ComputingQueryResponse &_computingQueryResponse, std::string &_xmlString);

protected:

    bool exportToFile(const Tucuxi::Core::ComputingResponse &_computingResponse, std::string _fileName);

    bool exportToString(const Tucuxi::Core::ComputingResponse &_computingResponse, std::string &_xmlString);

    bool exportAdjustment(const Tucuxi::Core::AdjustmentResponse *_prediction,
                          Tucuxi::Common::XmlNode &_rootNode);

    bool exportSinglePrediction(const Tucuxi::Core::SinglePredictionResponse *_prediction,
                          Tucuxi::Common::XmlNode &_rootNode);

    bool exportSinglePoints(const Tucuxi::Core::SinglePointsResponse *_prediction,
                          Tucuxi::Common::XmlNode &_rootNode);

    bool exportPercentiles(const Tucuxi::Core::PercentilesResponse *_prediction,
                          Tucuxi::Common::XmlNode &_rootNode);

    bool exportCycleData(const Tucuxi::Core::CycleData &_cycleData,
                          Tucuxi::Common::XmlNode &_rootNode);

    bool exportCycleDatas(const std::vector<Tucuxi::Core::CycleData> &_cycleDatas,
                          Tucuxi::Common::XmlNode &_rootNode);

    bool exportDosageHistory(const Tucuxi::Core::DosageHistory &_history,
            Tucuxi::Common::XmlNode &_rootNode);

    void addNode(Tucuxi::Common::XmlNode &_rootNode,
                 std::string _nodeName,
                 std::string _nodeValue);

    template<typename T>
    void addNode(Tucuxi::Common::XmlNode &_rootNode,
                 std::string _nodeName,
                 T _nodeValue)
    {
        Tucuxi::Common::XmlNode node = m_doc.createNode(
                    Tucuxi::Common::EXmlNodeType::Element, _nodeName, std::to_string(_nodeValue));
        _rootNode.addChild(node);
    }


    Tucuxi::Common::XmlDocument m_doc;
};

} // namespace Query
} // namespace Tucuxi

#endif // COMPUTINGRESPONSEXMLEXPORT_H