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
/// This class does not perform any calculation, it only embeds information.
///
class DrugTreatment
{
public:

    ///
    /// \brief Empty constructor
    ///
    DrugTreatment();


    ///
    /// \brief Gets the treatment dosage history
    /// \return The treatment dosage history, as a const reference
    ///
    const DosageHistory& getDosageHistory() const;


    ///
    /// \brief Gets the treatment dosage history
    /// \return The treatment dosage history, as a modifiable element
    ///
    DosageHistory& getModifiableDosageHistory();

    ///
    /// \brief Returns the patient covariates
    /// \return The patient covariates as a const reference
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

    ///
    /// \brief Adds a patient covariate to the treatment
    /// \param _covariate The new covariate
    ///
    void addCovariate(std::unique_ptr<PatientCovariate> _covariate);

    ///
    /// \brief Adds a dosage time range to the dosage history
    /// \param _timeRange The new dosage time range to add
    ///
    ///
    void addDosageTimeRange(std::unique_ptr<DosageTimeRange> _timeRange);

    ///
    /// \brief Adds a custom target
    /// \param _target The custom target to add
    ///
    void addTarget(std::unique_ptr<Target> _target);

    ///
    /// \brief Adds a sample measure
    /// \param _sample The sample measure to add
    ///
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

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DRUGTREATMENT_H
