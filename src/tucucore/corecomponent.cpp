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


bool CoreComponent::loadDrug()
{
    return false;
}


bool CoreComponent::loadTreatment()
{
    return false;
}


ConcentrationPredictionPtr CoreComponent::computeConcentrations(PredictionType _type,
    Tucuxi::Common::DateTime _start,
    Tucuxi::Common::DateTime _end,
    int _nbPoints)
{
    ConcentrationPredictionPtr result;

    IntakeSeries intakes;
    CovariateSeries covariates;
    ParametersSeries parameters;
    SampleSeries samples;
   
    // Extract intakes, covariates, parameters and samples
    IntakeExtractor::extract(m_treatment->getDosageHistory(), _start, _end, intakes);
    CovariateExtractor::extract(m_drug->getCovariates(), m_treatment->getCovariates(), _start, _end, covariates);
    ParametersExtractor::extract(covariates, _start, _end, parameters);
    SampleExtractor::extract(m_treatment->getSamples(), _start, _end, samples);

    return result;
}


PercentilesPredictionPtr CoreComponent::computePercentiles(PredictionType _type,
    Tucuxi::Common::DateTime _start,
    Tucuxi::Common::DateTime _end,
    const PercentileRanks &_ranks,
    int _nbPoints)
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

}
}
