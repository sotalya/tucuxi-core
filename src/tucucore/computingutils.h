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


#ifndef TUCUXI_CORE_COMPUTINGUTILS_H
#define TUCUXI_CORE_COMPUTINGUTILS_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/concentrationprediction.h"
#include "tucucore/covariateevent.h"
#include "tucucore/multiconcentrationprediction.h"

namespace Tucuxi {
namespace Core {

class DrugModel;
class ActiveMoiety;
class PkModelCollection;
class GeneralExtractor;

///
/// \brief The ComputingUtils class, shared among computing modules
/// This class offers method used by different modules like ComputingComponent,
/// MonteCarloPercentileCalculator and ComputingAdjustments.
/// It also stores data like the Pk models and the general extractor.
///
/// Everything is public within this class. The users have to take care not to
/// modify the object members.
///
class ComputingUtils
{
public:
    /// An empty constructor
    ComputingUtils();


    ///
    /// \brief computeActiveMoiety computes an active moiety from the analytes predictions
    /// \param _activeMoiety The active moiety that should be computed
    /// \param _analytesPredictions The analytes concentration predictions
    /// \param _activeMoietyPredictions The resulting active moiety concentrations
    /// \return ComputingResult::Ok if everything went well, ComputingResult::ActiveMoietyCalculationError else
    ///
    ComputingStatus computeActiveMoiety(
            const ActiveMoiety* _activeMoiety,
            const std::vector<ConcentrationPredictionPtr>& _analytesPredictions,
            ConcentrationPredictionPtr& _activeMoietyPredictions);



    /// The set of available Pk models, shared throughout the application
    std::shared_ptr<PkModelCollection> m_models;

    /// The general extractor that can be shared by various calculators
    std::unique_ptr<GeneralExtractor> m_generalExtractor;


    ///
    /// \brief getCovariateAtTime retrieves the covariate value at a specific date
    /// \param _date The date of interest
    /// \param _covariates A CovariateSeries
    /// \return A CovariateEvent corresponding to the covariate value at the specified date
    ///
    /// This function is currently usused
    ///
    CovariateEvent getCovariateAtTime(const DateTime& _date, const CovariateSeries& _covariates);

    ComputingStatus computeMultiActiveMoiety(
            const std::vector<ActiveMoiety*>& _activemoieties,
            const std::vector<MultiConcentrationPredictionPtr>& _analytesPredictions,
            MultiConcentrationPredictionPtr& _activeMoietyPredictions);
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGUTILS_H
