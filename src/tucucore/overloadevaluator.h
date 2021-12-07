#ifndef TUCUXI_CORE_OVERLOADEVALUATOR_H
#define TUCUXI_CORE_OVERLOADEVALUATOR_H

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/dosage.h"

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
    bool isAcceptable(IntakeSeries& _intakeSeries, const ComputingTrait* _trait);

    ///
    /// \brief Set the maximum values
    /// \param _nbPredictionPoints Maximum number of points for a prediction
    /// \param _nbPercentilesPoints Maximum number of points for percentiles
    /// \param _nbDosagePossibilities Maximum number of dosage possibilities for an adjustment
    ///
    void setValues(int _nbPredictionPoints, int _nbPercentilesPoints, int _nbDosagePossibilities);

    ///
    /// \brief get the error message if an overload is detected
    /// \return The error message
    ///
    /// This function should be called if isAcceptable() returns false to know why.
    ///
    std::string getErrorMessage() const;


private:
    ///  Maximum number of points for a prediction
    CycleSize m_nbPredictionPoints;

    /// Maximum number of points for percentiles
    CycleSize m_nbPercentilePoints;

    /// Maximum number of dosage possibilities for an adjustment
    int m_nbDosagePossibilities;

    /// Error message that is set when there is an overload detected
    std::string m_errorMessage;
};

class SingleOverloadEvaluator
{
public:
    static OverloadEvaluator* getInstance();
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_OVERLOADEVALUATOR_H
