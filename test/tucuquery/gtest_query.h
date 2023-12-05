//@@license@@

#ifndef GTEST_QUERY_H
#define GTEST_QUERY_H

#include <string>

#include "tucuquery/parametersdata.h"

using namespace Tucuxi;
using namespace Query;

class GtestTucuQuery{
public:
    std::unique_ptr<Tucuxi::Query::Treatment> importXml(std::string _xmlString);

    void exportXml(std::unique_ptr<Tucuxi::Query::Treatment> _pTreatment, std::string& _xmlString);

    void removeSpecialsCharacters(std::string& _exportedXmlString);

    void printImportExportString(std::string& _importString, std::string& _exportString);

};

#endif // GTEST_QUERY_H
