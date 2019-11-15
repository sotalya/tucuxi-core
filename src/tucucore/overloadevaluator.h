#ifndef TUCUXI_CORE_OVERLOADEVALUATOR_H
#define TUCUXI_CORE_OVERLOADEVALUATOR_H

#include "tucucore/dosage.h"
#include "tucucore/computingservice/computingrequest.h"

namespace Tucuxi {
namespace Core {

///
/// \brief The OverloadEvaluator class
/// This class shall be used to evaluate if a computation is too big and is
/// likely to result in a system crash.
///
/// It only offers a single method that returns a boolean
///
class OverloadEvaluator
{
public:

    ///
    /// \brief Empty constructor
    ///
    OverloadEvaluator();

    ///
    /// \brief Constructor with values
    /// \param _nbPredictionPoints Maximum number of points for a prediction
    /// \param _nbPercentilesPoints Maximum number of points for percentiles
    /// \param _nbDosagePossibilities Maximum number of dosage possibilities for an adjustment
    ///
    OverloadEvaluator(int _nbPredictionPoints, int _nbPercentilesPoints, int _nbDosagePossibilities);

    ///
    /// \brief Indicates if a calculation can be executed or not
    /// \param _intakeSeries The extracted intake series to be used
    /// \param _trait The procesing traits embedding the computation instructions
    /// \return true if the calculation is feasible, false else
    ///
    bool isAcceptable(IntakeSeries &_intakeSeries,
                      const ComputingTrait *_trait);

    ///
    /// \brief Set the maximum values
    /// \param _nbPredictionPoints Maximum number of points for a prediction
    /// \param _nbPercentilesPoints Maximum number of points for percentiles
    /// \param _nbDosagePossibilities Maximum number of dosage possibilities for an adjustment
    ///
    void setValues(int _nbPredictionPoints, int _nbPercentilesPoints, int _nbDosagePossibilities);


private:

    ///  Maximum number of points for a prediction
    int m_nbPredictionPoints;

    /// Maximum number of points for percentiles
    int m_nbPercentilePoints;

    /// Maximum number of dosage possibilities for an adjustment
    int m_nbDosagePossibilities;
};

class SingleOverloadEvaluator
{
public:

    static OverloadEvaluator* getInstance();
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_OVERLOADEVALUATOR_H
