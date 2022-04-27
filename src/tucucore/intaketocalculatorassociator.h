//@@lisence@@

#ifndef TUCUXI_CORE_INTAKETOCALCULATORASSOCIATOR_H
#define TUCUXI_CORE_INTAKETOCALCULATORASSOCIATOR_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/dosage.h"
#include "tucucore/pkmodel.h"

namespace Tucuxi {
namespace Core {


class IntakeToCalculatorAssociator
{
public:
    ///
    /// \brief associate Associates the calculators to the intakes
    /// \param _intakes List of intakes in time
    /// \param _pkModel The PkModel responsible to create the IntakeIntervalCalculators
    /// \return Ok if everything went well, UnsupportedRoute in case there is no calculator for the specified route
    /// This function should note create duplicates of IntakeIntervalCalculators. If the same routes are used, then
    /// a single calculator should be created
    static ComputingStatus associate(Tucuxi::Core::IntakeSeries& _intakes, const Tucuxi::Core::PkModel& _pkModel);
};

} // namespace Core
} // namespace Tucuxi
#endif // TUCUXI_CORE_INTAKETOCALCULATORASSOCIATOR_H
