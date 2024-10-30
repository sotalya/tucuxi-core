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


#ifndef TUCUXI_CORE_MULTIMONTECARLOPERCENTILECALCULATOR_H
#define TUCUXI_CORE_MULTIMONTECARLOPERCENTILECALCULATOR_H

#include "tucucore/computingservice/computingaborter.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/dosage.h"
#include "tucucore/montecarlopercentilecalculator.h"
#include "tucucore/parameter.h"
#include "tucucore/sampleevent.h"

#include "likelihood.h"

namespace Tucuxi {
namespace Core {

class IConcentrationCalculator;
class IResidualErrorModel;
class PercentilesPrediction;
class ActiveMoiety;


class IAprioriPercentileCalculatorMulti : public IPercentileCalculator
{
public:
    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    virtual ComputingStatus calculate(
            PercentilesPrediction& _percentiles,
            const DateTime& _recordFrom,
            const DateTime& _recordTo,
            const GroupsIntakeSeries& _intakes,
            const GroupsParameterSetSeries& _parameters,
            const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
            const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> >& _residualErrorModel,
            const std::map<AnalyteGroupId, Etas>& _etas,
            const PercentileRanks& _percentileRanks,
            IConcentrationCalculator& _concentrationCalculator,
            const ActiveMoiety* _activeMoiety,
            ComputingAborter* _aborter) = 0;

    virtual ~IAprioriPercentileCalculatorMulti() {}
};

class IAposterioriPercentileCalculatorMulti : public IPercentileCalculator
{
public:
    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    virtual ComputingStatus calculate(
            PercentilesPrediction& _percentiles,
            const DateTime& _recordFrom,
            const DateTime& _recordTo,
            const GroupsIntakeSeries& _intakes,
            const GroupsParameterSetSeries& _parameters,
            const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
            const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> >& _residualErrorModel,
            const std::map<AnalyteGroupId, Etas>& _etas,
            const std::map<AnalyteGroupId, SampleSeries>& _samples,
            const PercentileRanks& _percentileRanks,
            IConcentrationCalculator& _concentrationCalculator,
            const ActiveMoiety* _activeMoiety,
            ComputingAborter* _aborter) = 0;

    virtual ~IAposterioriPercentileCalculatorMulti() {}
};

class PercentileCalculatorMultiBase
{
protected:
    ///
    /// \brief calculateActiveMoietyAndSort
    /// \param _percentiles percentiles calculated within the method
    /// \param _percentileRanks List of percentiles ranks
    /// \param _activeMoiety Active moiety that has to be calculated
    /// \param _analyteGroupIds Analyte group Ids for the active moiety
    /// \param _recordedIntakes List of intakes that have to be saved
    /// \param _nbPatients Number of patients during Monte Carlo
    /// \param _concentrations Concentrations calculated during Monte Carlo
    /// \param _simpleCalculator A single-analytegroup calculator
    /// \param _times The times calculated
    /// \return ComputingResult::Ok if everything went well
    ///
    ComputingStatus calculateActiveMoietyAndSort(
            PercentilesPrediction& _percentiles,
            const PercentileRanks& _percentileRanks,
            const ActiveMoiety* _activeMoiety,
            const std::vector<AnalyteGroupId>& _analyteGroupIds,
            IntakeSeries _recordedIntakes,
            size_t _nbPatients,
            std::vector<std::vector<std::vector<std::vector<Concentration> > > >& _concentrations,
            MonteCarloPercentileCalculatorBase& _simpleCalculator,
            const std::vector<TimeOffsets>& _times);
};

class AprioriPercentileCalculatorMulti :
    public IAprioriPercentileCalculatorMulti,
    public MonteCarloPercentileCalculatorBase,
    public PercentileCalculatorMultiBase
{
public:
    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _percentileRanks List of percentiles ranks
    /// \param _activeMoiety Active moiety that has to be calculated
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ComputingStatus calculate(
            PercentilesPrediction& _percentiles,
            const DateTime& _recordFrom,
            const DateTime& _recordTo,
            const GroupsIntakeSeries& _intakes,
            const GroupsParameterSetSeries& _parameters,
            const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
            const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> >& _residualErrorModel,
            const std::map<AnalyteGroupId, Etas>& _etas,
            const PercentileRanks& _percentileRanks,
            IConcentrationCalculator& _concentrationCalculator,
            const ActiveMoiety* _activeMoiety,
            ComputingAborter* _aborter) override;

    ~AprioriPercentileCalculatorMulti() override {}
};


class AposterioriMonteCarloPercentileCalculatorMulti :
    public IAposterioriPercentileCalculatorMulti,
    public MonteCarloPercentileCalculatorBase,
    public PercentileCalculatorMultiBase
{
public:
    ///
    /// \brief calculate
    /// \param _percentiles percentiles calculated within the method
    /// \param _recordFrom Date from which we start recording the concentration
    /// \param _recordTo Date until which we record the concentration
    /// \param _intakes Intake series
    /// \param _parameters Initial parameters series
    /// \param _omega covariance matrix for inter-individual variability
    /// \param _residualErrorModel Residual error model
    /// \param _etas Etas pre-calculated by the aposteriori calculator
    /// \param _samples List of samples
    /// \param _percentileRanks List of percentiles ranks
    /// \param _aborter An aborter object allowing to abort the calculation
    /// \return The status of calculation
    ComputingStatus calculate(
            PercentilesPrediction& _percentiles,
            const DateTime& _recordFrom,
            const DateTime& _recordTo,
            const GroupsIntakeSeries& _intakes,
            const GroupsParameterSetSeries& _parameters,
            const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
            const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> >& _residualErrorModel,
            const std::map<AnalyteGroupId, Etas>& _etas,
            const std::map<AnalyteGroupId, SampleSeries>& _samples,
            const PercentileRanks& _percentileRanks,
            IConcentrationCalculator& _concentrationCalculator,
            const ActiveMoiety* _activeMoiety,
            ComputingAborter* _aborter) override;

    ~AposterioriMonteCarloPercentileCalculatorMulti() override {}
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_MULTIMONTECARLOPERCENTILECALCULATOR_H
