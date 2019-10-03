/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_COMPUTINGUTILS_H
#define TUCUXI_CORE_COMPUTINGUTILS_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/concentrationprediction.h"

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
    ComputingResult computeActiveMoiety(
            const ActiveMoiety *_activeMoiety,
            const std::vector<ConcentrationPredictionPtr> &_analytesPredictions,
            ConcentrationPredictionPtr &_activeMoietyPredictions);


    /// The set of available Pk models, shared throughout the application
    std::shared_ptr<PkModelCollection> m_models;

    /// The general extractor that can be shared by various calculators
    std::unique_ptr<GeneralExtractor> m_generalExtractor;

};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGUTILS_H
