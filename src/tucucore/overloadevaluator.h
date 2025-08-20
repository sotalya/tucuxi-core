/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_CORE_OVERLOADEVALUATOR_H
#define TUCUXI_CORE_OVERLOADEVALUATOR_H

#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/intakeevent.h"

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
    /// \brief Constructor with values
    /// \param _nbPredictionPoints Maximum number of points for a prediction
    /// \param _nbPercentilesPoints Maximum number of points for percentiles
    /// \param _nbDosagePossibilities Maximum number of dosage possibilities for an adjustment
    ///
    OverloadEvaluator(
            CycleSize _nbPredictionPoints = 10000,
            CycleSize _nbPercentilesPoints = 2000,
            int _nbDosagePossibilities = 10000);

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
    void setValues(CycleSize _nbPredictionPoints, CycleSize _nbPercentilesPoints, int _nbDosagePossibilities);

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
