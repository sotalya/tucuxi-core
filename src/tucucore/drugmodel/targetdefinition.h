/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TARGETDEFINITION_H
#define TARGETDEFINITION_H



#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {


class SubTargetDefinition : public PopulationValue
{
public:

    SubTargetDefinition(std::string _id, Value _value, Operation *_operation) :
        PopulationValue(_id, _value, _operation) {}

    SubTargetDefinition() : PopulationValue("",0, nullptr) {}

};


enum class TargetType {
    Residual,
    Peak,
    Mean,
    Auc,
    FullAuc
};


class TargetDefinition
{
public:

    TargetDefinition()
    {}

    TargetDefinition(TargetType _type,
                     SubTargetDefinition *_valueMin,
                     SubTargetDefinition *_valueMax,
                     SubTargetDefinition *_valueBest,
                     SubTargetDefinition *_tMin,
                     SubTargetDefinition *_tMax,
                     SubTargetDefinition *_tBest) :
        m_targetType(_type),
        m_valueMin(_valueMin),
        m_valueMax(_valueMax),
        m_valueBest(_valueBest),
        m_tMin(_tMin),
        m_tMax(_tMax),
        m_tBest(_tBest)
    {

    }

    TargetType getTargetType() const { return m_targetType;}

    const SubTargetDefinition & getCMin() const { return *m_valueMin;}
    const SubTargetDefinition & getCMax() const { return *m_valueMax;}
    const SubTargetDefinition & getCBest() const { return *m_valueBest;}
    const SubTargetDefinition & getTMin() const { return *m_tMin;}
    const SubTargetDefinition & getTMax() const { return *m_tMax;}
    const SubTargetDefinition & getTBest() const { return *m_tBest;}

//protected:
    TargetType m_targetType;

    std::unique_ptr<SubTargetDefinition> m_valueMin;
    std::unique_ptr<SubTargetDefinition> m_valueMax;
    std::unique_ptr<SubTargetDefinition> m_valueBest;
    std::unique_ptr<SubTargetDefinition> m_tMin;
    std::unique_ptr<SubTargetDefinition> m_tMax;
    std::unique_ptr<SubTargetDefinition> m_tBest;
};


typedef std::vector<std::unique_ptr<TargetDefinition>> TargetDefinitions;

}
}


#endif // TARGETDEFINITION_H
