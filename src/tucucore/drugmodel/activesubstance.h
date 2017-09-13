/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef ACTIVESUBSTANCE_H
#define ACTIVESUBSTANCE_H

#include <string>
#include <vector>

#include "analyte.h"

namespace Tucuxi {
namespace Core {

class ActiveSubstance
{
public:
    ActiveSubstance();

    void setPkModelId(std::string _pkModelId) { m_pkModelId = _pkModelId;}
    void addAnalyte(Analyte _analyte) { m_analytes.push_back(_analyte);}

    std::string m_pkModelId;
    std::vector<Analyte> m_analytes;
};



}
}


#endif // ACTIVESUBSTANCE_H
