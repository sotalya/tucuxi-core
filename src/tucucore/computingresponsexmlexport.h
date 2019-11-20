#ifndef COMPUTINGRESPONSEXMLEXPORT_H
#define COMPUTINGRESPONSEXMLEXPORT_H

#include "tucucore/computingservice/computingresponse.h"

#include "tucucommon/xmlnode.h"
#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"

namespace Tucuxi {
namespace Core {

class ComputingResponseXmlExport
{
public:
    ComputingResponseXmlExport();

    bool exportToFile(const ComputingResponse &_computingResponse, std::string _fileName);

    bool exportToString(const ComputingResponse &_computingResponse, std::string &_xmlString);

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

} // namespace Core
} // namespace Tucuxi

#endif // COMPUTINGRESPONSEXMLEXPORT_H
