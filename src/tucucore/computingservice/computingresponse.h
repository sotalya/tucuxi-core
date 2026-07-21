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


#ifndef TUCUXI_CORE_COMPUTINGRESPONSE_H
#define TUCUXI_CORE_COMPUTINGRESPONSE_H

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "tucucore/cyclestatistics.h"
#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/targetevaluationresult.h"

namespace Tucuxi {
namespace Core {


typedef struct
{
    std::string m_parameterId;
    Value m_value;
} ParameterValue;

typedef struct
{
    std::string m_covariateId;
    Value m_value;
} CovariateValue;


///
/// \brief The CycleData class, meant to embed data about a cycle
/// It contains concentrations and times for a single cycle (or interval)
/// Actually it does contains concentrations of one or more compartments,
/// allowing to store analytes and active moieties. The identification of
/// the analytes and active moieties is not internally stored by the CycleData,
/// it is the responsibility of the user to know what concentration stands each
/// analyte or active moiety.
///
class CycleData
{
public:
    CycleData() : m_start(DateTime::undefinedDateTime()), m_end(DateTime::undefinedDateTime()) {}
    CycleData(const Tucuxi::Common::DateTime& _start, const Tucuxi::Common::DateTime& _end, TucuUnit _unit)
        : m_start(_start), m_end(_end), m_unit(std::move(_unit))
    {
    }

    void addData(const TimeOffsets& _offsets, const Concentrations& _concentrations)
    {
        m_times.push_back(_offsets);
        m_concentrations.push_back(_concentrations);
    }

    const std::vector<TimeOffsets>& getTimes() const
    {
        return m_times;
    }

    const MultiCompConcentrations& getConcentrations() const
    {
        return m_concentrations;
    }

    /// \brief Absolute start time of the cycle
    Tucuxi::Common::DateTime m_start;

    /// \brief Absolute end time of the cycle
    /// This value could be computed from m_start and m_times, but it is more convenient
    /// To have it as a member variable
    Tucuxi::Common::DateTime m_end;

    /// \brief A serie of times expressed as offsets in hours to the start of a cycle
    std::vector<TimeOffsets> m_times;

    /// \brief A vector of vector of concentrations
    /// Each inner vector contains the concentrations of an analyte or a compartment
    /// The size of each inner vector has to be the same as m_times.
    MultiCompConcentrations m_concentrations;

    /// \brief Unit of concentrations
    /// The area under curve corresponds to this unit times hours
    TucuUnit m_unit;

    /// \brief Pk parameter values for this cycle
    /// Can be used or not to store the values of the Pk parameters used for this cycle
    std::vector<ParameterValue> m_parameters;

    /// \brief covariates values for this cycle
    /// Can be used or not to store the values of the covariates used for this cycle
    std::vector<CovariateValue> m_covariates;

    /// \brief The statistics about the cycle data
    /// For each compartment or analyte, the statistics.
    CycleStats m_statistics;
};

class SinglePredictionData;
class SinglePointsData;
class AdjustmentData;
class PercentilesData;


// \brief Measurement prediction error --- used to estimate the Mean Prediction
//        Error and the Mean Absolute Prediction Error.
class MeasurePredError
{
public:
    ///
    /// \brief Initialize the Measurement prediction error.
    /// \param _measure Actual measure
    /// \param _prediction Prediction from the model
    /// \param _predError Prediction error
    /// \param _absPredErrorPct Absolute prediction error (percentage)
    ///
    MeasurePredError(
            Value const& _measure, Value const& _prediction, Value const& _predError, Value const& _absPredErrorPct)
        : m_measure{_measure}, m_prediction{_prediction}, m_predError{_predError}, m_absPredErrorPct{_absPredErrorPct}
    {
    }

    ///
    /// \brief Return the measure.
    /// \return Current measure.
    Value getMeasure() const
    {
        return m_measure;
    }

    ///
    /// \brief Return the prediction.
    /// \return Current prediction.
    Value getPrediction() const
    {
        return m_prediction;
    }

    ///
    /// \brief Return the prediction error.
    /// \return Current prediction error.
    Value getPredictionError() const
    {
        return m_predError;
    }

    ///
    /// \brief Return the absolute prediction error (in percentage).
    /// \return Current absolute prediction error (in percentage).
    Value getAbsPredErrorPct() const
    {
        return m_absPredErrorPct;
    }

protected:
    Value m_measure;
    Value m_prediction;
    Value m_predError;
    Value m_absPredErrorPct;
};


/// \brief Goodness-of-Fit values for the performed computations.
class GofData
{
public:
    ///
    /// \brief Initialize the Goodness-of-Fit object.
    /// \param _mae MAE evaluation metric
    /// \param _mape MAPE evaluation metric
    /// \param _mse MSE evaluation metric
    /// \param _rmse RMSE evaluation metric
    /// \param _rmsle RMSLE evaluation metric
    /// \param _rrmse RRMSE evaluation metric
    /// \param _rSquared R-squared evaluation metric
    /// \param _predErrors Measurement prediction errors
    /// \param _meanPredictionError Mean prediction error
    /// \param _meanAbsolutePredictionError Mean absolute prediction error
    /// \param _unit Unit of the concentrations used for the GoF computations
    ///
    GofData(Value const& _mae,
            Value const& _mape,
            Value const& _mse,
            Value const& _rmse,
            Value const& _rmsle,
            Value const& _rrmse,
            Value const& _rSquared,
            std::vector<MeasurePredError> _predErrors,
            Value const& _meanPredictionError,
            Value const& _meanAbsolutePredictionError,
            TucuUnit _unit)
        : m_mae{_mae}, m_mape{_mape}, m_mse{_mse}, m_rmse{_rmse}, m_rmsle{_rmsle}, m_rrmse{_rrmse},
          m_rSquared{_rSquared}, m_predErrors{std::move(_predErrors)}, m_meanPredictionError{_meanPredictionError},
          m_meanAbsolutePredictionError{_meanAbsolutePredictionError}, m_unit{std::move(_unit)}
    {
    }

    ///
    /// \brief Return the value of the MAE statistic.
    /// \return Computed value of the MAE statistic.
    Value getMae() const
    {
        return m_mae;
    }

    ///
    /// \brief Return the value of the MAPE statistic.
    /// \return Computed value of the MAPE statistic.
    Value getMape() const
    {
        return m_mape;
    }

    ///
    /// \brief Return the value of the MSE statistic.
    /// \return Computed value of the MSE statistic.
    Value getMse() const
    {
        return m_mse;
    }

    ///
    /// \brief Return the value of the RMSE statistic.
    /// \return Computed value of the RMSE statistic.
    Value getRmse() const
    {
        return m_rmse;
    }

    ///
    /// \brief Return the value of the RMSLE statistic.
    /// \return Computed value of the RMSLE statistic.
    Value getRmsle() const
    {
        return m_rmsle;
    }

    ///
    /// \brief Return the value of the RRMSE statistic.
    /// \return Computed value of the RRMSE statistic.
    Value getRrmse() const
    {
        return m_rrmse;
    }

    ///
    /// \brief Return the value of the R-squared statistic.
    /// \return Computed value of the R-squared statistic.
    Value getRSquared() const
    {
        return m_rSquared;
    }

    ///
    /// \brief Return the measurement prediction errors.
    /// \return Measurement prediction errors.
    std::vector<MeasurePredError> getPredErrors() const
    {
        return m_predErrors;
    }

    ///
    /// \brief Return the mean prediction error.
    /// \return Computed value of mean prediction error.
    Value getMeanPredictionError() const
    {
        return m_meanPredictionError;
    }

    ///
    /// \brief Return the mean absolute prediction error.
    /// \return Computed value of mean absolute prediction error.
    Value getMeanAbsolutePredictionError() const
    {
        return m_meanAbsolutePredictionError;
    }

    ///
    /// \brief Return the unit used for the GoF computations.
    /// \return Unit.
    TucuUnit getUnit() const
    {
        return m_unit;
    }


protected:
    /// MAE evaluation metric.
    Value m_mae;
    /// MAPE evaluation metric.
    Value m_mape;
    /// MSE evaluation metric.
    Value m_mse;
    /// RMSE evaluation metric.
    Value m_rmse;
    /// RMSLE evaluation metric.
    Value m_rmsle;
    /// RRMSE evaluation metric.
    Value m_rrmse;
    /// R-squared evaluation metric.
    Value m_rSquared;
    /// Measurement prediction errors.
    std::vector<MeasurePredError> m_predErrors;
    /// Mean prediction error.
    Value m_meanPredictionError;
    /// Mean absolute prediction error.
    Value m_meanAbsolutePredictionError;
    /// Unit used for the GoF computations.
    TucuUnit m_unit;
};


///
/// \brief The ComputedData class is the base class for every response
/// It contains all the computed data
///
class ComputedData
{
public:
    virtual ~ComputedData() = 0;
    RequestResponseId getId() const;

    void setGof(const GofData& _data)
    {
        m_gofData = _data;
    }

    /// \brief Retrieve the Goodness-of-Fit data from the computed data.
    /// This will return nullptr for most of the implementations, since GoF data
    /// cannot be computed for most of them. This is done on purpose --- whenever
    /// it is non-null, it makes sense to access the different statistics and
    /// give them in the report.
    /// Compute GoF if supported.
    /// Default implementation does nothing.
    GofData const* getGof() const noexcept
    {
        return m_gofData ? &*m_gofData : nullptr;
    }

protected:
    ComputedData(RequestResponseId _id);

    RequestResponseId m_id;

    /// \brief Goodness-of-Fit data.
    mutable std::optional<GofData> m_gofData;
};


/// \brief The SinglePointsResponse class
/// This class contains data generated by a ComputingTraitSinglePoints, that is
/// when values at specific times are asked by a request.
/// It is also the response for ComputingTraitAtMeasures, that calculates
/// points at the measure times found in the DrugTreatment.
/// Therefore it offers the absolute times corresponding to the request as well
/// as the calculated concentrations at these points.
///
class SinglePointsData : public ComputedData
{
public:
    SinglePointsData(RequestResponseId _id) : ComputedData(std::move(_id)) {}

    /// Absolute time of each concentration
    std::vector<Tucuxi::Common::DateTime> m_times;

    /// Concentration values
    /// It is a vector of concentrations. It is divided by compartment and then by value in time:
    /// m_concentrations[compartmentIndex][timeIndex]
    /// So, for a single analyte we will typically have a [0][nbTimes] array, which is more
    /// efficient than having the opposite
    std::vector<Concentrations> m_concentrations;

    /// Unit of concentrations
    TucuUnit m_unit;

    void setLogLikelihood(Value _value)
    {
        m_logLikelihood = _value;
    }
    Value getLogLikelihood() const
    {
        return m_logLikelihood;
    }

    void addCompartmentInfo(const CompartmentInfo& _info)
    {
        m_infos.push_back(_info);
    }

    const std::vector<CompartmentInfo>& getCompartmentInfos() const
    {
        return m_infos;
    }

protected:
    ///
    /// \brief likelihood of the parameters based on the samples, in case of a posteriori predictions
    ///
    /// This value is 0.0 in case of a priori predictions
    ///
    Value m_logLikelihood{0.0};

    std::vector<CompartmentInfo> m_infos;
};


class ConcentrationData
{
public:
    void addCycleData(const CycleData& _data)
    {
        m_data.push_back(_data);
    }
    const std::vector<CycleData>& getData() const
    {
        return m_data;
    }
    std::vector<CycleData>& getModifiableData()
    {
        return m_data;
    }

    void addCompartmentInfo(const CompartmentInfo& _info)
    {
        m_infos.push_back(_info);
    }

    const std::vector<CompartmentInfo>& getCompartmentInfos() const
    {
        return m_infos;
    }

protected:
    std::vector<CycleData> m_data;
    std::vector<CompartmentInfo> m_infos;
};

///
/// \brief The SinglePredictionResponse class
/// It contains data of a single prediction, as a vector of CycleData.
class SinglePredictionData : public ComputedData, public ConcentrationData
{
public:
    SinglePredictionData(RequestResponseId _id) : ComputedData(std::move(_id)) {}

    void setLogLikelihood(Value _value)
    {
        m_logLikelihood = _value;
    }

    Value getLogLikelihood() const
    {
        return m_logLikelihood;
    }

protected:
    ///
    /// \brief likelihood of the parameters based on the samples, in case of a posteriori predictions
    ///
    /// This value is 0.0 in case of a priori predictions
    ///
    Value m_logLikelihood{0.0};
};

///
/// \brief The DosageAdjustment class
/// This class embeds all information about a potential dosage adjustment:
/// The dosage history, the score (suitability of the dosage), and concentrations
/// if the concentrations have been calculated
///
class DosageAdjustment : public ConcentrationData
{
public:
    DosageAdjustment() = default;

    double getGlobalScore() const
    {
        if (m_targetsEvaluation.empty()) {
            return 0.0;
        }
        double sum = 0.0;
        for (const auto& target : m_targetsEvaluation) {
            sum += target.getScore();
        }
        return sum / static_cast<double>(m_targetsEvaluation.size());
    }

    DosageHistory getDosageHistory() const
    {
        return m_history;
    }

    DosageHistory m_history;
    std::vector<TargetEvaluationResult> m_targetsEvaluation;
};

///
/// \brief The AdjustmentResponse class
/// This class embeds a vector of potential adjustments, each one being a
/// Dosage history, a score, and optionally a concentration prediction.
///
class AdjustmentData : public SinglePredictionData
{
public:
    AdjustmentData(RequestResponseId _id) : SinglePredictionData(std::move(_id)) {}

    void addAdjustment(const DosageAdjustment& _adjustment)
    {
        m_adjustments.push_back(_adjustment);
    }

    void setAdjustments(const std::vector<DosageAdjustment>& _adjustments)
    {
        m_adjustments = _adjustments;
    }

    const std::vector<DosageAdjustment>& getAdjustments() const
    {
        return m_adjustments;
    }

    // To be checked if we need that function instead of the previous one
    // std::vector<DosageAdjustment> getAdjustments() const { return m_adjustments;}

    const DosageAdjustment& getCurrentDosageWithScore() const
    {
        return m_currentDosageWithScore;
    }

    void setCurrentDosageWithScore(DosageAdjustment _currentDosage)
    {
        m_currentDosageWithScore = std::move(_currentDosage);
    }

    void setIsCurrentInRange(bool _isInRange)
    {
        m_isCurrentInRange = _isInRange;
    }

    bool isCurrentInRange() const
    {
        return m_isCurrentInRange;
    }

protected:
    /// A vector of possible dosage adjustments
    std::vector<DosageAdjustment> m_adjustments;

    /// Embeds the current dosage and its score/target attainment
    DosageAdjustment m_currentDosageWithScore;

    /// Indicates if the current dosage is in the targets range
    bool m_isCurrentInRange{false};
};

struct EtodaPointResult
{
    double m_measuredConc{0.0};
    double m_trueConc{0.0};
    double m_samplingHour{0.0};

    bool m_adjustmentFound{false};
    int m_zoneLabel{0};
    double m_metricValue{0.0};
};

struct EtodaHourResult
{
    double m_samplingHour{0.0};
    std::vector<EtodaPointResult> m_points;
};

class EtodaData : public ComputedData
{
public:
    EtodaData(RequestResponseId _id) : ComputedData(std::move(_id)) {}

    void addEtodaHourResult(const Tucuxi::Core::EtodaHourResult& _result)
    {
        m_results.push_back(_result);
    }

    void setEtodaResults(const std::vector<EtodaHourResult>& _results)
    {
        m_results = _results;
    }

    const std::vector<EtodaHourResult>& getEtodaResults() const
    {
        return m_results;
    }

private:
    std::vector<EtodaHourResult> m_results;
};

///
/// \brief The PercentilesResponse class
/// It shall contain different percentiles, for a certain period of time.
/// In order to embed all data necessary for correct exploitation, it contains:
/// 1. The percentile ranks as a vector of doubles, each one being in [0.0,100.0]
/// 2. The concentration of percentiles, as a vector of CycleMultiData,
///    one CycleData per percentile
///
class PercentilesData : public ComputedData
{
public:
    PercentilesData(RequestResponseId _id) : ComputedData(std::move(_id)) {}

    void setRanks(const PercentileRanks& _ranks)
    {
        m_ranks = _ranks;
    }

    size_t getNbRanks() const
    {
        return m_ranks.size();
    }

    PercentileRank getRank(unsigned int _index) const
    {
        return m_ranks[_index];
    }

    void addPercentileData(const std::vector<CycleData>& _data)
    {
        m_data.push_back(_data);
    }

    const CycleData& getData(size_t _percentileIndex, size_t _cycleIndex) const
    {
        return m_data[_percentileIndex][_cycleIndex];
    }

    const std::vector<CycleData>& getPercentileData(size_t _percentileIndex) const
    {
        return m_data[_percentileIndex];
    }

    const PercentileRanks& getRanks() const
    {
        return m_ranks;
    }

    void setNbPointsPerHour(double _nbPointsPerHour)
    {
        m_nbPointsPerHour = _nbPointsPerHour;
    }
    double getNbPointsPerHour() const
    {
        return m_nbPointsPerHour;
    }

    void addCompartmentInfo(const CompartmentInfo& _info)
    {
        m_infos.push_back(_info);
    }

    const std::vector<CompartmentInfo>& getCompartmentInfos() const
    {
        return m_infos;
    }

private:
    std::vector<std::vector<CycleData> > m_data;
    PercentileRanks m_ranks;
    double m_nbPointsPerHour{0.0};
    std::vector<CompartmentInfo> m_infos;
};


///
/// \brief The ComputingResponse class
/// It is the response to a ComputingRequest object. It has an identifier and
/// a vector of SingleComputingResponse, and as such can embed various responses,
/// like a prediction, various percentiles, and a dosage adjustment.
///
class ComputingResponse
{
public:
    ///
    /// \brief ComputingResponse constructor
    /// \param _id The Id of the request
    ///
    ComputingResponse(RequestResponseId _id);

    ///
    /// \brief get the Id of the request
    /// \return the Id of the request
    ///
    RequestResponseId getId() const;

    ///
    /// \brief Adds a response to this list of responses
    /// \param _response The response to be added, as a unique_ptr
    ///
    void addResponse(std::unique_ptr<ComputedData> _response);

    ///
    /// \brief Gets the vector of responses
    /// \return A const reference to the vector of responses
    ///
    // std::vector<std::unique_ptr<SingleComputingResponse> > & getResponses() { return m_responses;}
    // const std::vector<std::unique_ptr<SingleComputingResponse> > & getResponses() const { return m_responses;}

    const ComputedData* getData() const
    {
        return m_data.get();
    }

    std::unique_ptr<ComputedData> getUniquePointerData()
    {
        return std::move(m_data);
    }

    ///
    /// \brief Set the computing time of this request
    /// \param _computingTime the computing time of the request
    ///
    void setComputingTimeInSeconds(std::chrono::duration<double> _computingTime);

    ///
    /// \brief get the computing time of this request
    /// \return the computing time of the request
    ///
    std::chrono::duration<double> getComputingTimeInSeconds() const;

    ///
    /// \brief set the computing status
    /// \param _result The computing status
    ///
    void setComputingStatus(ComputingStatus _result);

    ComputingStatus getComputingStatus() const;

protected:
    /// Id of the request
    RequestResponseId m_id;

    std::unique_ptr<ComputedData> m_data;

    std::chrono::duration<double, std::ratio<1, 1> > m_computingTimeInSeconds{0};

    ComputingStatus m_computingResult{ComputingStatus::Undefined};
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGRESPONSE_H
