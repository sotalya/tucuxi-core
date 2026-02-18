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


#include "computinggof.h"

#include "computingservice/computingresponse.h"


namespace Tucuxi {
namespace Core {


void ComputingGof::compute(
        ComputingRequest const& _computingRequest,
        SinglePredictionData const& _predData,
        std::optional<GofData>& _gofData)
{
    auto const& drugTreatment = _computingRequest.getDrugTreatment();
    auto const& requestSamples = drugTreatment.getSamples();

    std::vector<Value> computedValues;
    std::vector<Value> measuredValues;

    Tucuxi::Common::TucuUnit const modelUnit = _computingRequest.getDrugModel().getActiveMoieties()[0]->getUnit();
    for (auto const& requestSample : requestSamples) {
        auto const& sampleDate = requestSample->getDate();

        // Seek the corresponding computed value among the predictions.
        Tucuxi::Common::TucuUnit compUnit;
        auto const computedValue = findValueAt(_predData.getData(), sampleDate, compUnit);

        if (!computedValue) {
            continue;
        }
        // Enqueue the value computed according to the model.
        computedValues.push_back(
                Tucuxi::Common::UnitManager::convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        computedValue.value(), compUnit, modelUnit));
        // Enqueue the sample value in the appropriate unit.
        measuredValues.push_back(
                Tucuxi::Common::UnitManager::convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        requestSample->getValue(), requestSample->getUnit(), modelUnit));
    }
    computeGofStatistics(computedValues, measuredValues, _gofData);
}


void ComputingGof::compute(
        ComputingRequest const& _computingRequest,
        SinglePointsData const& _pointsData,
        std::optional<GofData>& _gofData)
{
    auto const& drugTreatment = _computingRequest.getDrugTreatment();
    auto const& requestSamples = drugTreatment.getSamples();

    std::vector<Value> computedValues;
    std::vector<Value> measuredValues;

    Tucuxi::Common::TucuUnit const modelUnit = _computingRequest.getDrugModel().getActiveMoieties()[0]->getUnit();
    size_t const N = _pointsData.m_times.size();
    assert(requestSamples.size() == N);

    for (size_t i = 0; i < N; ++i) {
        auto const& sampleDate = requestSamples.at(i)->getDate();
        assert(sampleDate == _pointsData.m_times[i]);

        // Enqueue the value computed according to the model.
        computedValues.push_back(
                Tucuxi::Common::UnitManager::convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        _pointsData.m_concentrations[0][i], _pointsData.m_unit, modelUnit));
        // Enqueue the sample value in the appropriate unit.
        measuredValues.push_back(
                Tucuxi::Common::UnitManager::convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        requestSamples.at(i)->getValue(), requestSamples.at(i)->getUnit(), modelUnit));
    }
    computeGofStatistics(computedValues, measuredValues, _gofData);
}


void ComputingGof::computeGofStatistics(
        std::vector<Value> const& _computedValues,
        std::vector<Value> const& _measuredValues,
        std::optional<GofData>& _gofData)
{
    assert(_computedValues.size() == _measuredValues.size());

    Value const mae = computeMae(_computedValues, _measuredValues);
    Value const mape = computeMape(_computedValues, _measuredValues);
    Value const mse = computeMse(_computedValues, _measuredValues);
    Value const rmse = computeRmse(_computedValues, _measuredValues);
    Value const rmsle = computeRmsle(_computedValues, _measuredValues);
    Value const rrmse = computeRrmse(_computedValues, _measuredValues);
    Value const rSquared = computeRSquared(_computedValues, _measuredValues);
    Value meanPredictionError;
    Value meanAbsolutePredictionError;
    std::vector<MeasurePredError> const predErrors = computeMeasurePredErrors(
            _computedValues, _measuredValues, meanPredictionError, meanAbsolutePredictionError);

    _gofData.emplace(
            mae,
            mape,
            mse,
            rmse,
            rmsle,
            rrmse,
            rSquared,
            std::move(predErrors),
            meanPredictionError,
            meanAbsolutePredictionError);
}


std::optional<Value> ComputingGof::findValueAt(
        std::vector<CycleData> const& _cycles,
        Tucuxi::Common::DateTime const& _queryTime,
        Tucuxi::Common::TucuUnit& _unit,
        std::size_t _analyteIndex)
{
    _unit = Tucuxi::Common::TucuUnit();
    for (auto const& cycle : _cycles) {
        if (_queryTime < cycle.m_start || _queryTime > cycle.m_end) {
            continue;
        }
        double const offsetHours = (_queryTime - cycle.m_start).toHours();

        if (cycle.m_times.empty() || _analyteIndex >= cycle.m_times.size()
            || _analyteIndex >= cycle.m_concentrations.size()) {
            return std::nullopt;
        }

        TimeOffsets const& times = cycle.m_times[_analyteIndex];
        auto const& concs = cycle.m_concentrations[_analyteIndex];

        if (times.size() != concs.size() || times.empty()) {
            return std::nullopt;
        }
        auto it = std::lower_bound(times.begin(), times.end(), offsetHours);

        _unit = cycle.m_unit;

        // Case A: before first sample.
        if (it == times.begin()) {
            return concs.front();
        }

        // Case B: after last sample.
        if (it == times.end()) {
            return concs.back();
        }

        // Case C: exact match.
        if (*it == offsetHours) {
            std::size_t idx = std::distance(times.begin(), it);

            return concs[idx];
        }

        // Case D: linear interpolation.
        std::size_t idx1 = std::distance(times.begin(), it) - 1;
        std::size_t idx2 = idx1 + 1;

        double const t1 = times[idx1];
        double const t2 = times[idx2];
        Value const c1 = concs[idx1];
        Value const c2 = concs[idx2];

        Value const alpha = (offsetHours - t1) / (t2 - t1);

        return c1 + alpha * (c2 - c1);
    }

    return std::nullopt;
}


Value ComputingGof::computeMae(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues)
{
    size_t const n = _computedValues.size();
    Value mae = 0;
    for (size_t i = 0; i < n; ++i) {
        mae += std::abs(_measuredValues[i] - _computedValues[i]);
    }
    if (n > 0) {
        mae /= static_cast<Value>(n);
    }

    return mae;
}


Value ComputingGof::computeMape(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues)
{
    size_t const n = _computedValues.size();
    Value mape = 0;
    bool const tinyValueFound = std::any_of(
            _measuredValues.begin(), _measuredValues.end(), [](Value _x) { return std::abs(_x) < 1000 * m_valueEps; });

    if (tinyValueFound) {
        mape = m_plusInf;
    }
    else {
        mape = 0;
        for (size_t i = 0; i < n; ++i) {
            mape += std::abs((_measuredValues[i] - _computedValues[i]) / _measuredValues[i]);
        }
        if (n > 0) {
            mape /= static_cast<Value>(n);
        }
    }

    return mape;
}


Value ComputingGof::computeMse(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues)
{
    size_t const n = _computedValues.size();
    Value mse = 0;
    for (size_t i = 0; i < n; ++i) {
        Value const diff = _measuredValues[i] - _computedValues[i];
        mse += diff * diff;
    }
    if (n > 0) {
        mse /= static_cast<Value>(n);
    }
    return mse;
}


Value ComputingGof::computeRmse(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues)
{
    return std::sqrt(computeMse(_computedValues, _measuredValues));
}


Value ComputingGof::computeRmsle(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues)
{
    // RMSLE is undefined when any value is negative (log of a non-positive number).
    bool const hasNegativeValue =
            std::any_of(_computedValues.begin(), _computedValues.end(), [](Value _x) { return _x < -m_valueEps; })
            || std::any_of(_measuredValues.begin(), _measuredValues.end(), [](Value _x) { return _x < -m_valueEps; });

    if (hasNegativeValue) {
        return m_plusInf;
    }

    size_t const n = _computedValues.size();
    Value rmsle = 0;
    for (size_t i = 0; i < n; ++i) {
        Value const logDiff = std::log(_computedValues[i] + 1.0) - std::log(_measuredValues[i] + 1.0);
        rmsle += logDiff * logDiff;
    }
    if (n > 0) {
        rmsle = std::sqrt(rmsle / static_cast<Value>(n));
    }

    return rmsle;
}


Value ComputingGof::computeRrmse(std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues)
{
    size_t const n = _measuredValues.size();
    if (n == 0) {
        return 0;
    }

    Value meanMeasured = 0;
    for (size_t i = 0; i < n; ++i) {
        meanMeasured += std::abs(_measuredValues[i]);
    }
    meanMeasured /= static_cast<Value>(n);

    if (meanMeasured < 1000 * m_valueEps) {
        return m_plusInf;
    }

    return computeRmse(_computedValues, _measuredValues) / meanMeasured;
}


Value ComputingGof::computeRSquared(
        std::vector<Value> const& _computedValues, std::vector<Value> const& _measuredValues)
{
    size_t const n = _computedValues.size();
    Value rSquared = 0;
    Value measuredValsMean = 0;
    Value diffMeasuredComputedSq = 0;
    for (size_t i = 0; i < n; ++i) {
        diffMeasuredComputedSq += (_measuredValues[i] - _computedValues[i]) * (_measuredValues[i] - _computedValues[i]);
        measuredValsMean += _measuredValues[i];
    }
    measuredValsMean /= static_cast<Value>(n);
    Value diffWithMeanSq = 0;
    for (size_t i = 0; i < n; ++i) {
        diffWithMeanSq += (_measuredValues[i] - measuredValsMean) * (_measuredValues[i] - measuredValsMean);
    }

    if (diffWithMeanSq < 1000 * m_valueEps) {
        if (diffMeasuredComputedSq < 1000 * m_valueEps) {
            rSquared = 1;
        }
        else {
            rSquared = m_negInf;
        }
    }
    else {
        rSquared = 1 - diffMeasuredComputedSq / diffWithMeanSq;
    }

    return rSquared;
}


std::vector<MeasurePredError> ComputingGof::computeMeasurePredErrors(
        std::vector<Value> const& _computedValues,
        std::vector<Value> const& _measuredValues,
        Value& _meanPredictionError,
        Value& _meanAbsolutePredictionError)
{
    std::vector<MeasurePredError> predErrors;
    size_t const n = _computedValues.size();

    _meanPredictionError = 0;
    _meanAbsolutePredictionError = 0;
    for (size_t i = 0; i < n; ++i) {
        Value const predError = _measuredValues[i] - _computedValues[i];
        _meanPredictionError += predError;
        Value const absPredErrorPct = std::abs(_computedValues[i]) > 1e3 * m_valueEps
                                              ? std::abs(predError / _computedValues[i]) * 100
                                              : m_plusInf;
        _meanAbsolutePredictionError += absPredErrorPct;

        MeasurePredError mpe(_measuredValues[i], _computedValues[i], predError, absPredErrorPct);
        predErrors.push_back(mpe);
    }

    _meanPredictionError /= static_cast<Value>(n);
    _meanAbsolutePredictionError /= static_cast<Value>(n);

    return predErrors;
}


} // namespace Core
} // namespace Tucuxi
