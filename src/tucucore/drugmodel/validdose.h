/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef VALIDDOSE_H
#define VALIDDOSE_H

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

typedef std::vector<Value> MultiAnalyteDose;

class ValidDoses
{
public:
    ValidDoses(Unit _unit, MultiAnalyteDose _defaultDose);

    virtual ~ValidDoses() {};

    Unit getUnit() const;
    MultiAnalyteDose getDefaultDose() const;

    virtual std::vector<MultiAnalyteDose> getDoses() const = 0;

    const std::vector<std::string>& getAnalyteIds() const;

    void setAnalyteIds(std::vector<std::string> _analyteIds);

protected:

    Unit m_unit;
    MultiAnalyteDose m_defaultDose;
    std::vector<std::string> m_analyteIds;
};

class AnyDoses : public ValidDoses
{
public:
    AnyDoses(Unit _unit, MultiAnalyteDose _defaultDose, MultiAnalyteDose _from, MultiAnalyteDose _to, MultiAnalyteDose _step);

    std::vector<MultiAnalyteDose> getDoses() const override;

protected:
    MultiAnalyteDose m_from;
    MultiAnalyteDose m_to;
    MultiAnalyteDose m_step;
};


class SpecificDoses : public ValidDoses
{

public:

    SpecificDoses(Unit _unit, MultiAnalyteDose _defaultDose);

    std::vector<MultiAnalyteDose> getDoses() const override;

protected:
    std::vector<MultiAnalyteDose> m_doses;
};

}
}


#endif // VALIDDOSE_H
