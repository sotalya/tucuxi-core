/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/drugtreatment.h"

namespace Tucuxi {
namespace Core {

DrugTreatment::DrugTreatment()
{
}


const DosageHistory& DrugTreatment::getDosageHistory() const
{
    return m_dosageHistory;
}


const Covariates& DrugTreatment::getCovariates() const
{
    return m_covariates;
}


const Samples& DrugTreatment::getSamples() const
{
    return m_samples;
}


}
}