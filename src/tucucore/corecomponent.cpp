/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/corecomponent.h"
#include "tucucore/drugmodel.h"
#include "tucucore/drugtreatment.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/covariateextractor.h"
#include "tucucore/targetextractor.h"
#include "tucucore/sampleextractor.h"
#include "tucucore/parametersextractor.h"
#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {


CoreComponent::CoreComponent(const std::string &_filename)
{
    registerInterface(dynamic_cast<IDataModelServices*>(this));
    registerInterface(dynamic_cast<IProcessingServices*>(this));
}


CoreComponent::~CoreComponent()
{
}


bool CoreComponent::loadDrug(const std::string& _xmlDrugDescription)
{
    return false;
}


bool CoreComponent::loadTreatment(const std::string& _xmlTreatmentDescription)
{
    return false;
}


ConcentrationPredictionPtr CoreComponent::computeConcentrations(const ConcentrationRequest &_request)
{
    ConcentrationPredictionPtr prediction;

    IntakeSeries intakes;
    CovariateSeries covariates;
    ParameterSetSeries parameters;
       
    // Extract intakes, covariates, parameters and samples
    IntakeExtractor::extract(*m_treatment->getDosageHistory(_request.getType() != PredictionType::Population), _request.getStart(), _request.getEnd(), intakes);
    CovariateExtractor::extract(m_drug->getCovariates(), m_treatment->getCovariates(), _request.getStart(), _request.getEnd(), covariates);
    ParametersExtractor::extract(covariates, _request.getStart(), _request.getEnd(), parameters);

    if (intakes.size() == 0) {
        m_logger.debug("No intakes, no calc.");
        return nullptr;
    }

    prediction = std::make_unique<ConcentrationPrediction>();

    switch (_request.getType())
    {
        case PredictionType::Population:
            computePopulation(prediction, _request.getNbPoints(), intakes, parameters);
            break;

        case PredictionType::Apiori:
            computeApriori(prediction, _request.getNbPoints(), intakes, parameters);
            break;

        case PredictionType::Aposteriori: {
            DateTime lastSampleDate;
            SampleSeries samples;
            SampleExtractor::extract(m_treatment->getSamples(), _request.getStart(), _request.getEnd(), samples);
            if (samples.size() > 0) {
                SampleSeries::iterator _smpend = samples.end();
                lastSampleDate = _smpend->getEventTime();
            }
            IntakeSeries intakesForEtas;
            IntakeExtractor::extract(*m_treatment->getDosageHistory(_request.getType() != PredictionType::Population), _request.getStart(), lastSampleDate, intakesForEtas);

            /// TODO YJE
            Etas etas;
            Omega omega;
            ResidualErrorModel residualErrorModel;
            extractError(m_drug->getErrorModel(), m_drug->getParemeters(), omega, residualErrorModel);
            computeAposterioriEtas(intakesForEtas, parameters, omega, residualErrorModel, samples, etas);
            computeAposteriori(prediction, _request.getNbPoints(), intakes, parameters, etas);
            break;
        }

        default:
            break;
    }

    return prediction;
}


PercentilesPredictionPtr CoreComponent::computePercentiles(const PercentilesRequest &_request)
{
    PercentilesPredictionPtr result;
    return result;
}


void CoreComponent::computeAdjustments()
{
}


Tucuxi::Common::Interface* CoreComponent::getInterface(const std::string &_name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}


ComputationResult CoreComponent::computeConcentrations(
    ConcentrationPredictionPtr &_prediction,
    const int _nbPoints,
    const IntakeSeries &_intakes,
    const ParameterSetSeries &_parameterSets,
    const Etas &_etas,
    const ResidualErrorModel &_residualErrorModel,
    const Deviation& _eps,
    const bool _isFixedDensity)
{   
    // The size of residuals vectors equals the number of compartments. This shouldnt be hardcoded here.
    Residuals r1(3);
    Residuals r2(3);

    // Go through all intakes
    for (IntakeSeries::const_iterator it = _intakes.begin(); it != _intakes.end(); it++) {                
        const IntakeEvent &intake = *it;
        
        // Get parameters at intake start time
        // For population calculation, could be done only once at the beginning        
        ParameterSetEventPtr parameters = _parameterSets.getAtTime(intake.getEventTime(), _etas);
        if (parameters == nullptr) {
            m_logger.error("No parameters found!");
            return ComputationResult::Failure;
        }
        
        // Use nbpoints for the minimum pt density
        // Must use an odd number
        const int density = static_cast<int>((int(floor(intake.getNbPoints())) % 2 != 0) ? floor(intake.getNbPoints()) : ceil(intake.getNbPoints()));

        // Compute concentrations for the current cycle        
        TimeOffsets times;
        Concentrations values;
        _prediction->allocate(density, times, values);
        IntakeIntervalCalculator::Result result = it->calculateIntakePoints(times, values, intake, *parameters, r1, intake.getNbPoints(), r2, _isFixedDensity);
        switch (result)
        {
            case IntakeIntervalCalculator::Result::Ok:
                break;
            case IntakeIntervalCalculator::Result::DensityError:
                // Restart computation with more points...

                // If nbpoints has changed (initial density was not the final density), change the density in the intakeevent
                // so it can be used if this intake is recalculated.
                //if (it->getNbPoints() != ink.getNbPoints()) {
                //    intake_series_t::index<tags::times>::type& intakes_times_index = intakes.get<tags::times>();
                //    intakes_times_index.modify(it, IntakeEvent::change_density(ink.nbPoints));
                //}
                break;
            default:
                m_logger.error("Failed in calculation with given parameter values.");
                return ComputationResult::Failure;
        }


        // Apply the intra-individual error
        if (!_residualErrorModel.isEmpty()) {
            _residualErrorModel.applyEpsToArray(values, _eps);
        }

        // Append computed values to our prediction
        _prediction->appendConcentrations(times, values);

        // Prepare residuals for the next cycle
        r1 = r2;
    }

    return ComputationResult::Success;
}


}
}
