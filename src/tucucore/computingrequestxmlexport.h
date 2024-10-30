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

#ifndef COMPUTINGREQUESTXMLEXPORT_H
#define COMPUTINGREQUESTXMLEXPORT_H

#include "tucucommon/xmlattribute.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"

#include "tucucore/computingservice/computingrequest.h"


class GTestTucuQuery;

namespace Tucuxi {
namespace Core {

class ComputingRequestXmlExport
{
public:
    ComputingRequestXmlExport();

    virtual ~ComputingRequestXmlExport();

    bool exportToString(const ComputingRequest& _computingRequest, std::string& _xmlString);

    bool exportToFile(const ComputingRequest& _computingRequest, const std::string& _fileName);

protected:
    bool exportDosageHistory(const Tucuxi::Core::DosageHistory& _history, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosageTimeRange(
            const std::unique_ptr<Tucuxi::Core::DosageTimeRange>& _timeRange, Tucuxi::Common::XmlNode& _rootNode);

    bool exportAbstractDosage(const Tucuxi::Core::Dosage& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::DosageBounded& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::DosageLoop& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::DosageSteadyState& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::DosageRepeat& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::DosageSequence& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::ParallelDosageSequence& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::LastingDose& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::DailyDose& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    bool exportDosage(const Tucuxi::Core::WeeklyDose& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    void exportFormulationAndRoute(const Tucuxi::Core::SingleDose& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    virtual void exportDose(const Tucuxi::Core::SingleDose& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    void exportSingleDose(const Tucuxi::Core::SingleDose& _dosage, Tucuxi::Common::XmlNode& _rootNode);

    void exportComputingOption(const Tucuxi::Core::ComputingOption& _option, Tucuxi::Common::XmlNode& _rootNode);

    std::string toString(const Tucuxi::Core::Formulation& _formulation);

    std::string toString(const Tucuxi::Core::AdministrationRoute& _administrationRoute);

    std::string toString(const Tucuxi::Core::AbsorptionModel& _absorptionModel);

    std::string toString(DataType _nodeValue);

    std::string toString(PredictionParameterType _nodeValue);

    std::string toString(TargetType _nodeValue);

    std::string toString(CompartmentsOption _nodeValue);

    std::string toString(BestCandidatesOption _nodeValue);

    std::string toString(LoadingOption _nodeValue);

    std::string toString(RestPeriodOption _nodeValue);

    std::string toString(SteadyStateTargetOption _nodeValue);

    std::string toString(TargetExtractionOption _nodeValue);

    std::string toString(FormulationAndRouteSelectionOption _nodeValue);

    std::string toString(RetrieveStatisticsOption _nodeValue);

    std::string toString(RetrieveParametersOption _nodeValue);

    std::string toString(RetrieveCovariatesOption _nodeValue);

    std::string toString(ForceUgPerLiterOption _nodeValue);

    std::string toString(bool _nodeValue);

    ///
    /// In case a custom toString() function does not exist use
    /// std::to_string. It will be usefull for basic types:
    /// int, double, ...
    ///
    template<typename T>
    std::string toString(T _nodeValue)
    {
        return std::to_string(_nodeValue);
    }

    void addNode(Tucuxi::Common::XmlNode& _rootNode, const std::string& _nodeName, const std::string& _nodeValue);

    void addNode(Tucuxi::Common::XmlNode& _rootNode, const std::string& _nodeName, const char* _nodeValue);

    ///
    /// Entry point for each node value. It uses toString()
    /// to convert the type to a string
    ///
    template<typename T>
    void addNode(Tucuxi::Common::XmlNode& _rootNode, const std::string& _nodeName, T _nodeValue)
    {
        Tucuxi::Common::XmlNode node =
                m_doc.createNode(Tucuxi::Common::EXmlNodeType::Element, _nodeName, toString(_nodeValue));
        _rootNode.addChild(node);
    }

    Tucuxi::Common::XmlDocument m_doc;

private:
    friend GTestTucuQuery;
};

} // namespace Core
} // namespace Tucuxi

#endif // COMPUTINGREQUESTXMLEXPORT_H
