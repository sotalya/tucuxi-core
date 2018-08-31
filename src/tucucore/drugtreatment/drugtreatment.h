/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGTREATMENT_H
#define TUCUXI_CORE_DRUGTREATMENT_H

#include "tucucore/dosage.h"
#include "tucucore/covariateevent.h"
#include "tucucore/drugtreatment/sample.h"
#include "tucucore/drugtreatment/target.h"

namespace Tucuxi {
namespace Core {

///
/// \brief The DrugTreatment represents all data about a patient treatment
/// It embeds the dosage history for a specific drug, the patient covariates,
/// the samples measured, and possible specific targets defined by a practician.
/// Typically it is meant to be given to the processing service.
/// It is typically constructed from a XML description that could be sent via
/// a REST interface or via a file in command line.
///
class DrugTreatment
{
public:

    ///
    /// \brief Empty constructor
    ///
    DrugTreatment();

    ///
    /// \brief TBD
    /// \param _useAdjustments
    /// \return TBD
    ///
    const DosageHistory& getDosageHistory(bool _useAdjustments) const;

    ///
    ////// \brief Returns the patient covariates
    ////// \return The patient covariates as a const reference
    ///
    const PatientVariates& getCovariates() const;

    ///
    /// \brief Returns the measures samples
    /// \return The measures samples as a const reference
    ///
    const Samples& getSamples() const;

    ///
    /// \brief Returns the specific targets
    /// \return The specific targest as a const reference
    /// In most use cases, there won't be any specific targets.
    ///
    const Targets& getTargets() const;


    void addCovariate(std::unique_ptr<PatientCovariate> _covariate);
    void addDosageTimeRange(std::unique_ptr<DosageTimeRange> _timeRange);
    void addTarget(std::unique_ptr<Target> _target);
    void addSample(std::unique_ptr<Sample> _sample);
    
private:

    /// The patient dosage history
    DosageHistory m_dosageHistory;

    /// The patient covariates
    PatientVariates m_covariates;

    /// The specific targets (often empty)
    Targets m_targets;

    /// The measures samples
    Samples m_samples;

    /// The dosage adjustments... TODO : Check if we really need that here. I don't think so
    DosageHistory m_adjustment;

    /// The date of adjustment. TODO : Check if we really need that here. I don't think so
    DateTime m_adjustmentDate;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DRUGTREATMENT_H
