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

#ifndef TUCUXI_CORE_COMPUTINGGOF_H
#define TUCUXI_CORE_COMPUTINGGOF_H

#include <limits>
#include <optional>
#include <vector>

#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

class CycleData;
class MeasurePredError;
class GofData;
class SinglePointsData;
class SinglePredictionData;

///
/// \brief Compute GoF statistics from measured and computed values.
///
class ComputingGof
{
public:
    ///
    /// \brief Perform Goodness-of-Fit computations.
    /// \param _computingRequest Computing request details
    /// \param _predData Single prediction data
    /// \param _gofData Resulting GoF statistics
    ///
    void compute(
            ComputingRequest const& _computingRequest,
            SinglePredictionData const& _predData,
            std::optional<GofData>& _gofData);

    ///
    /// \brief Perform Goodness-of-Fit computations.
    /// \param _computingRequest Computing request details
    /// \param _pointsData Single points data
    /// \param _gofData Resulting GoF statistics
    ///
    void compute(
            ComputingRequest const& _computingRequest,
            SinglePointsData const& _pointsData,
            std::optional<GofData>& _gofData);

protected:
    ///
    /// \brief Find the computed value at a specified time in a set of cycles.
    /// \param _cycles Set of cycles to explore
    /// \param _queryTime Sought time instant
    /// \param _unit Unit of measure (only meaningful if the returned value is
    ///        not nullopt)
    /// \param _analyteIndex Analyte index
    /// \return Sought value if present, nullopt if time is outside the ranges.
    static std::optional<Value> findValueAt(
            std::vector<CycleData> const& _cycles,
            Tucuxi::Common::DateTime const& _queryTime,
            Tucuxi::Common::TucuUnit& _unit,
            std::size_t _analyteIndex = 0);

    ///
    /// \brief Compute the set of GoF statistics.
    /// \param _computedValues Values computed by the model.
    /// \param _measuredValues Corresponding samples.
    /// \param _gofData Resulting GoF statistics
    static void computeGofStatistics(
            std::vector<Value> const& _computedValues,
            std::vector<Value> const& _measuredValues,
            std::optional<GofData>& _gofData);

    ///
    /// \brief Compute the Mean Absolute Error (MAE) evaluation metric.
    /// \param _computedValues Values computed by the model.
    /// \param _measuredValues Corresponding samples.
    /// \returns Computed MAE statistic.
    static Value computeMae(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues);

    ///
    /// \brief Compute the Mean Absolute Percentage Error (MAPE) evaluation
    ///        metric.
    /// \param _computedValues Values computed by the model.
    /// \param _measuredValues Corresponding samples.
    /// \returns Computed MAPE statistic.
    static Value computeMape(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues);

    ///
    /// \brief Compute the MSE evaluation metric.
    /// \param _computedValues Values computed by the model.
    /// \param _measuredValues Corresponding samples.
    /// \returns Computed MSE statistic.
    static Value computeMse(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues);
    ///
    /// \brief Compute the RMSE evaluation metric.
    /// \param _computedValues Values computed by the model.
    /// \param _measuredValues Corresponding samples.
    /// \returns Computed RMSE statistic.
    static Value computeRmse(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues);

    ///
    /// \brief Compute the Root Mean Squared Logarithmic Error (RMSLE) evaluation metric.
    /// \param _computedValues Values computed by the model.
    /// \param _measuredValues Corresponding samples.
    /// \returns Computed RMSLE statistic, or +infinity if any value is negative.
    static Value computeRmsle(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues);

    ///
    /// \brief Compute the R-squared evaluation metric.
    /// \param _computedValues Values computed by the model.
    /// \param _measuredValues Corresponding samples.
    /// \returns Computed R-squared statistic.
    static Value computeRSquared(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues);

    ///
    /// \brief Compute the measurement prediction errors.
    /// \param _computedValues Values computed by the model.
    /// \param _measuredValues Corresponding samples.
    /// \param _meanPredictionError Mean prediction error computed by the function.
    /// \param _meanAbsolutePredictionError Mean absolute prediction error computed by the function.
    /// \returns Computed measurement prediction error.
    static std::vector<MeasurePredError> computeMeasurePredErrors(
            std::vector<Value> const& _computedValues,
            std::vector<Value> const& _measuredValues,
            Value& _meanPredictionError,
            Value& _meanAbsolutePredictionError);

    /// Machine-epsilon for the Value type.
    inline static constexpr Value m_valueEps = std::numeric_limits<Value>::epsilon();
    /// Positive inifinity constant for the Value type.
    inline static constexpr Value m_plusInf = std::numeric_limits<Value>::infinity();
    /// Negative inifinity constant for the Value type.
    inline static constexpr Value m_negInf = -std::numeric_limits<Value>::infinity();
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGGOF_H
