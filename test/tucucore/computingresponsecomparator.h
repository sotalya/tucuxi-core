//@@license@@

#ifndef COMPUTINGRESPONSECOMPARATOR_H
#define COMPUTINGRESPONSECOMPARATOR_H

#include "tucucore/computingservice/computingresponse.h"

#include "tucucore/dosage.h"

void compareDosageHistory(const Tucuxi::Core::DosageHistory& _d1, const Tucuxi::Core::DosageHistory& _d2);

class ComputingResponseComparator
{
public:
    void compare(Tucuxi::Core::ComputingStatus _s1, Tucuxi::Core::ComputingStatus _s2);

    void compare(Tucuxi::Core::ComputingResponse& _r1, Tucuxi::Core::ComputingResponse& _r2);

};

#endif // COMPUTINGRESPONSECOMPARATOR_H
