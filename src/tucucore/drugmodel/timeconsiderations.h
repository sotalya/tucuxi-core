#ifndef TIMECONSIDERATIONS_H
#define TIMECONSIDERATIONS_H

#include <memory>
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

class HalfLife
{

private:
    Unit m_unit;
    // stdAprioriValue m_value;
    double m_multiplier;

};

class OutdatedMeasure
{

private:
    Unit m_unit;
    // stdAprioriValue m_value;


};

class TimeConsiderations
{
public:
    TimeConsiderations();

private:

    std::unique_ptr<HalfLife> m_halfLife;
    std::unique_ptr<OutdatedMeasure> m_outdatedMeasure;
};

}
}

#endif // TIMECONSIDERATIONS_H
