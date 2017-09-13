/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef ANALYTE_H
#define ANALYTE_H

#include <string>

namespace Tucuxi {
namespace Core {



class Analyte
{
public:
    Analyte();

    void setName(std::string _name) { m_name = _name;}
    void setAnalyteId(std::string _analyteId) { m_analyteId = _analyteId;}

    std::string m_name;
    std::string m_analyteId;

};

}
}


#endif // ANALYTE_H
