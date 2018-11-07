/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef COMPUTINGTRAIT_H
#define COMPUTINGTRAIT_H

#include "tucucommon/datetime.h"

#include "tucucore/definitions.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/computingservice/computingaborter.h"


namespace Tucuxi {
namespace Core {

///
/// \brief The PredictionParameterType enum
/// This enum represents the type of Pk parameters to use for computation.
/// It can be:
/// 1. Population : Only the default parameters values are used
/// 2. Apriori : The covariates modify the parameters
/// 3. Aposteriori : The covariates and the existing measures modify the parameters
///
enum class PredictionParameterType {
    Population = 0, //!< Population parameters
    Apriori,        //!< Covariates modify the parameters
    Aposteriori     //!< Covariates and mesaures modify the parameters
};

///
/// \brief The CompartmentsOption enum
/// This enum allows to select if only the main compartment concentration has to be
/// calculated or if the concentrations of all compartments are required
enum class CompartmentsOption {
    MainCompartment = 0, //!< Only interested in the main compartment
    AllCompartments      //!< Interested in all compartments
};

///
/// \brief The ComputingOption class
/// This class embeds some general options for computation. It is used by all requests.
/// Currently it offers choice for the type of parameters and which compartment we are
/// interested in.
///
class ComputingOption {
public:

    ///
    /// \brief ComputingOption Simple constructor
    /// \param _parameterType Type of parameters (population, aPriori, aPosteriori)
    /// \param _compartmentsOption What compartments are of interest (main or all)
    ///
    ComputingOption(
            PredictionParameterType _parameterType,
            CompartmentsOption _compartmentsOption);

    ///
    /// \brief getParametersType Gets the type of parameters
    /// \return The type of parameters
    ///
    PredictionParameterType getParametersType() const { return m_parameterType;}

    ///
    /// \brief getCompartmentsOption Gets what compartment is of interest
    /// \return The type of compartments of interest
    ///
    CompartmentsOption getCompartmentsOption() const { return m_compartmentsOption;}
protected:

    //! Type of parameters
    PredictionParameterType m_parameterType;

    //! What compartments are of interest
    CompartmentsOption m_compartmentsOption;
};


class ComputingComponent;

///
/// \brief The ComputingTrait class
/// This is the base class for all traits. It only has an identifier.
///
class ComputingTrait
{
public:
    ///
    /// \brief get the Id of the ComputingTrait
    /// \return The Id
    ///
    RequestResponseId getId() const;

    ///
    /// \brief ~ComputingTrait virtual destructor
    ///
    virtual ~ComputingTrait();
    
protected:
    ///
    /// \brief ComputingTrait constructor
    /// \param _id Id of the ComputingTrait
    /// The constructor is protected, as this class shall not be instanciated.
    /// Only subclasses are relevant.
    ///
    ComputingTrait(RequestResponseId _id);

    //! Id of the request
    RequestResponseId m_id;

private:
    friend class ComputingComponent;

    ///
    /// \brief compute Calls the compute() method in ComputingComponent
    /// \param _computingComponent The computing component that will do the computing job
    /// \param _request The request that has to be processed
    /// \param _response The response list in which we will add the new response
    /// \return ComputingResult::Success if everything went well, ComputingResult::Error else
    /// This abstract function has to be overriden in each and every real class.
    /// It allows the use of a visitor pattern from the ComputingComponent that can then
    /// iterate on the list of ComputingRequest for starting the computations.
    ///
    virtual ComputingResult compute(ComputingComponent &_computingComponent,
                                    const ComputingRequest &_request,
                                    std::unique_ptr<ComputingResponse> &_response) const = 0;
};


///
/// \brief The ComputingTraitStandard class
/// This is a base class for other Traits. It embeds information about:
/// 1. Start date of prediction calculation
/// 2. End date of prediction calculation
/// 3. Number of points for the calculation
/// 4. Some processing options (type of parameters, what compartments)
/// It shall not be instanciated, and this fact is assured by the protected constructor.
///
class ComputingTraitStandard : public ComputingTrait
{
public:

    //! Emtpy destructor
    ~ComputingTraitStandard() override;

    ///
    /// \brief getComputingOption Get the computing options
    /// \return The computing options
    ///
    ComputingOption getComputingOption() const;

    ///
    /// \brief getStart Get the start time of the range of interest
    /// \return The start time of the range of interest
    ///
    const Tucuxi::Common::DateTime& getStart() const;

    ///
    /// \brief getEnd Get the end time of the range of interest
    /// \return The end time of the range of interest
    ///
    const Tucuxi::Common::DateTime& getEnd() const;

    ///
    /// \brief getNbPointsPerHour Get the number of points to be calculated per hour
    /// \return The number of points to be calculated per hour
    /// This value is used by the IntakeExtractor to calculate the number of points
    /// to be calculated for each intake
    ///
    double getNbPointsPerHour() const;

protected:

    ///
    /// \brief ComputingTraitStandard A simple protected constructor
    /// \param _id Id of the request
    /// \param _start Start date of prediction calculation
    /// \param _end End date of prediction calculation
    /// \param _nbPointsPerHour Requested number of points per hour
    /// \param _computingOption Some processing options (type of parameters, what compartments)
    /// The constructor is protected, so that this class can not be directly created
    ///
    ComputingTraitStandard(RequestResponseId _id,
                            Tucuxi::Common::DateTime _start,
                            Tucuxi::Common::DateTime _end,
                            double _nbPointsPerHour,
                            ComputingOption _computingOption);

private:

    //! Some processing options (type of parameters, what compartments)
    ComputingOption m_computingOption;

    //! Start date of prediction calculation
    Tucuxi::Common::DateTime m_start;

    //! End date of prediction calculation
    Tucuxi::Common::DateTime m_end;


    //! _nbPointsPerHour Requested number of points per hour
    double m_nbPointsPerHour;

};

///
/// \brief The ComputingTraitSinglePoints class
/// This class embeds the information required to compute concentrations at
/// specific time points.
///
class ComputingTraitSinglePoints : public ComputingTrait
{
public:

    ///
    /// \brief ComputingTraitSinglePoints Simple constructor
    /// \param _id Id of the request
    /// \param _times A vector of times of interest
    /// \param _computingOption Computing options (parameters type, compartments)
    ///
    ComputingTraitSinglePoints(RequestResponseId _id,
                                std::vector<Tucuxi::Common::DateTime> _times,
                                ComputingOption _computingOption);

    const std::vector<Tucuxi::Common::DateTime> &getTimes() const;

    ///
    /// \brief getComputingOption Get the computing options
    /// \return The computing options
    ///
    ComputingOption getComputingOption() const;

protected:

    //! Vector of the times of interest
    std::vector<Tucuxi::Common::DateTime> m_times;

    //! Some processing options (type of parameters, what compartments)
    ComputingOption m_computingOption;

private:

    ///
    /// \brief compute Calls the compute() method in ComputingComponent
    /// \param _computingComponent The computing component that will do the computing job
    /// \param _request The request that has to be processed
    /// \param _response The response list in which we will add the new response
    /// \return ComputingResult::Success if everything went well, ComputingResult::Error else
    ///
    ComputingResult compute(ComputingComponent &_computingComponent,
                            const ComputingRequest &_request,
                            std::unique_ptr<ComputingResponse> &_response) const override;

    friend ComputingComponent;
};

///
/// \brief The ComputingTraitAtMeasures class
/// This class shall be used for computing concentrations at the times of
/// the DrugTreatment measures. Typically used for comparing the measured
/// concentrations with a posteriori predictions.
///
class ComputingTraitAtMeasures : public ComputingTrait
{
public:

    ///
    /// \brief ComputingTraitAtMeasures Simple constructor
    /// \param _id Id of the request
    /// \param _computingOption Computing options (parameters type, compartments)
    ///
    ComputingTraitAtMeasures(RequestResponseId _id, ComputingOption _computingOption);

    ///
    /// \brief getComputingOption Get the computing options
    /// \return The computing options
    ///
    ComputingOption getComputingOption() const;

protected:

    //! Some processing options (type of parameters, what compartments)
    ComputingOption m_computingOption;

private:

    ///
    /// \brief compute Calls the compute() method in ComputingComponent
    /// \param _computingComponent The computing component that will do the computing job
    /// \param _request The request that has to be processed
    /// \param _response The response list in which we will add the new response
    /// \return ComputingResult::Success if everything went well, ComputingResult::Error else
    ///
    ComputingResult compute(ComputingComponent &_computingComponent,
                            const ComputingRequest &_request,
                            std::unique_ptr<ComputingResponse> &_response) const override;
};

///
/// \brief The BestCandidatesOption enum
/// This option allow to ask for a single dosage adjustement or for all acceptable adjustments
enum class BestCandidatesOption
{
    /// Only return the best dosage
    BestDosage = 0,

    /// Return all acceptable dosages
    AllDosages
};

///
/// \brief The ChargingOption enum
/// This option allows to authorize or not a charging dose to rapidly attain steady state
///
enum class ChargingOption
{
    /// No charging dose is allowed
    NoChargingDose = 0,

    /// A charging dose shall be proposed if relevant
    ChargingDoseAllowed
};

///
/// \brief The RestPeriodOption enum
/// This option allows to authorize or not a resting period to rapidly attain steady state
enum class RestPeriodOption
{
    /// No resting period is allowed
    NoRestPeriod = 0,

    /// A resting period shall be proposed if relevant
    RestPeriodAllowed
};

///
/// \brief The SteadyStateTargetOption enum
/// This indicates if the target shall be evaluated at steady state or within the original
/// drug treatment time range
enum class SteadyStateTargetOption
{
    /// Target evaluated at steady state
    AtSteadyState = 0,

    /// Target evaluated within the original time range
    WithinTreatmentTimeRange

};

///
/// \brief The TargetExtactionOption enum
/// This enum describes the way the targets shall be extracted
enum class TargetExtractionOption
{
    /// Forces the population values to be used
    PopulationValues = 0,

    /// Forces the a priori values to be calculated and used
    AprioriValues,

    /// Only use the individual targets
    IndividualTargets,

    /// Use the individual target, and if for an active moiety and a target type no
    /// individual target exists, then use the definition
    DefinitionIfNoIndividualTarget
};

///
/// \brief The FormulationAndRouteSelectionOption enum
/// This enum describes the options for selecting the candidate formulation and routes
///
enum class FormulationAndRouteSelectionOption
{
    /// Use only the last formulation and route used in the current treatment
    /// If the treatment is empty, then use the default formulation and route of the
    /// drug model
    LastFormulationAndRoute = 0,

    /// Use only the default formulation and route of the drug model
    DefaultFormulationAndRoute,

    /// Use all available formulation and routes of the drug model
    AllFormulationAndRoutes
};

///
/// \brief The ComputingTraitAdjustment class
/// This class embeds all information required for computing adjustments. It can return
/// potential dosages, and future concentration calculations, depending on the options.
///
/// If nbPoints = 0, then no curve will be returned by the computation, only the dosages
///
class ComputingTraitAdjustment : public ComputingTraitStandard
{
public:
    ///
    /// \brief ComputingTraitAdjustment
    /// \param _id
    /// \param _start
    /// \param _end
    /// \param _nbPointsPerHour Requested number of points per hour
    /// \param _nbPoints
    /// \param _computingOption
    /// \param _adjustmentTime
    /// \param _adjustmentOption
    /// \param _chargingOption
    /// \param _restPeriodOption
    /// \param _steadyStateTargetOption
    /// \param _targetExtractionOption
    /// \param _formulationAndRouteSelectionOption
    /// If nbPoints = 0, then no curve will be returned by the computation, only the dosages
    ///
    ComputingTraitAdjustment(RequestResponseId _id,
                             Tucuxi::Common::DateTime _start,
                             Tucuxi::Common::DateTime _end,
                             double _nbPointsPerHour,
                             ComputingOption _computingOption,
                             Tucuxi::Common::DateTime _adjustmentTime,
                             BestCandidatesOption _adjustmentOption,
                             ChargingOption _chargingOption,
                             RestPeriodOption _restPeriodOption,
                             SteadyStateTargetOption _steadyStateTargetOption,
                             TargetExtractionOption _targetExtractionOption,
                             FormulationAndRouteSelectionOption _formulationAndRouteSelectionOption);

    ///
    /// \brief getAdjustmentTime Get the time of adjustment
    /// \return Time of the adjustment
    ///
    Tucuxi::Common::DateTime getAdjustmentTime() const;
    BestCandidatesOption getBestCandidatesOption() const;
    FormulationAndRouteSelectionOption getFormulationAndRouteSelectionOption() const;
    SteadyStateTargetOption getSteadyStateTargetOption() const;
    ChargingOption getChargingOption() const;
    RestPeriodOption getRestPeriodOption() const;
    TargetExtractionOption getTargetExtractionOption() const;

protected:

    //! Date of the adjustment
    Tucuxi::Common::DateTime m_adjustmentTime;

    //! Adjustment options : only the best, or all possible ones
    BestCandidatesOption m_adjustmentOption;

    ChargingOption m_chargingOption;

    RestPeriodOption m_restPeriodOption;

    SteadyStateTargetOption m_steadyStateTargetOption;

    TargetExtractionOption m_targetExtractionOption;

    FormulationAndRouteSelectionOption m_formulationAndRouteSelectionOption;

private:

    ///
    /// \brief compute Calls the compute() method in ComputingComponent
    /// \param _computingComponent The computing component that will do the computing job
    /// \param _request The request that has to be processed
    /// \param _response The response list in which we will add the new response
    /// \return ComputingResult::Success if everything went well, ComputingResult::Error else
    ///
    ComputingResult compute(ComputingComponent &_computingComponent,
                            const ComputingRequest &_request,
                            std::unique_ptr<ComputingResponse> &_response) const override;
};

///
/// \brief The ComputingTraitConcentration class
/// This class represents a request for a single prediction
///
class ComputingTraitConcentration : public ComputingTraitStandard
{
public:

    ///
    /// \brief ComputingTraitConcentration Simple constructor
    /// \param _id Id of the request
    /// \param _start Start time of the range to be calculated
    /// \param _end End time of the range to be calculated
    /// \param _nbPointsPerHour Requested number of points per hour
    /// \param _computingOption Some computing options
    ///
    ComputingTraitConcentration(RequestResponseId _id,
                                 Tucuxi::Common::DateTime _start,
                                 Tucuxi::Common::DateTime _end,
                                 double _nbPointsPerHour,
                                 ComputingOption _computingOption);
private:

    ///
    /// \brief compute Calls the compute() method in ComputingComponent
    /// \param _computingComponent The computing component that will do the computing job
    /// \param _request The request that has to be processed
    /// \param _response The response list in which we will add the new response
    /// \return ComputingResult::Success if everything went well, ComputingResult::Error else
    ///
    ComputingResult compute(ComputingComponent &_computingComponent,
                            const ComputingRequest &_request,
                            std::unique_ptr<ComputingResponse> &_response) const override;
};


///
/// \brief The ComputingTraitPercentiles class
/// This class embeds all information for calculating percentiles. Additionnaly to
/// standard single predictions, it requires the list of asked percentiles.
/// This list is a vector of values in [0.0, 100.0]
///
class ComputingTraitPercentiles : public ComputingTraitStandard
{
public:

    ///
    /// \brief ComputingTraitPercentiles Simple constructor
    /// \param _id Id of the request
    /// \param _start Start time of the range to be calculated
    /// \param _end End time of the range to be calculated
    /// \param _ranks The percentile ranks as a vector of double
    /// \param _nbPointsPerHour Requested number of points per hour
    /// \param _computingOption Some computing options
    /// \param _aborter An aborter objet to to cancel computation
    ///
    ComputingTraitPercentiles(
            RequestResponseId _id,
            Tucuxi::Common::DateTime _start,
            Tucuxi::Common::DateTime _end,
            const PercentileRanks &_ranks,
            double _nbPointsPerHour,
            ComputingOption _computingOption,
            ComputingAborter *_aborter = nullptr);

    ///
    /// \brief getRanks Retrieves the vector of percentile ranks
    /// \return The vector of percentiles ranks
    ///
    const PercentileRanks &getRanks() const { return m_ranks;}

    ///
    /// \brief getAborter Retrieves the aborter
    /// \return The current aborter that should be used by the computing engine
    ///
    ComputingAborter *getAborter() const { return m_aborter;}

private:

    //! A vector of percentile ranks
    PercentileRanks m_ranks;

    //! An aborter to cancel current computing
    ComputingAborter *m_aborter;

private:

    ///
    /// \brief compute Calls the compute() method in ComputingComponent
    /// \param _computingComponent The computing component that will do the computing job
    /// \param _request The request that has to be processed
    /// \param _response The response list in which we will add the new response
    /// \return ComputingResult::Success if everything went well, ComputingResult::Error else
    ///
    ComputingResult compute(
            ComputingComponent &_computingComponent,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response) const override;
};


///
/// \brief The ComputingTraits class
/// This class is a collection of ComputingTrait. It is meant to group various
/// calculations that will be applied on a single DrugTreatment and a single
/// DrugModel.
/// For instance, if a client needs a prediction, percentiles, and a proposition
/// of dosage adjustment, then three ComputingTrait will be added to a ComputingTraits
///
/// The ComputingTrait objects are stored as a vector of unique_ptr on ComputingTrait.
///
/// An iterator is supplied to access the ComputingTraits instead of a standard getter.
///
/// Example:
///
/// \code
/// ComputingTraits::Iterator it = computingTraits.begin();
/// ComputingResult result = ComputingResult::Success;
/// while (it != computingTraits.end()) {
///     ComputingTraits *traits = it->get();
///     // Do whatever needed with the iterator
///
///     it++;
/// }
/// \endcode
///
class ComputingTraits
{
public:

    ///
    /// \brief addTrait Adds a ComputingTrait to the list
    /// \param _trait The ComputingTrait to pass, as a unique_ptr
    /// Passing a unique pointer is mandatory here, so the caller should use
    /// std::move() to give ownership to the ComputingTraits.
    ///
    /// For instance:
    ///
    /// \code
    /// // Creation of a unique pointer
    /// std::unique_ptr<Tucuxi::Core::ComputingTrait> computingTrait = std::make_unique<Tucuxi::Core::ComputingTraitConcentration>(
    ///             m_requestID,
    ///             _type,
    ///             _startDate,
    ///             _endDate,
    ///             nbPoints,
    ///             Tucuxi::Core::ComputingOption::MainCompartment);
    ///
    /// Tucuxi::Core::ComputingTraits traits;
    ///
    /// // Addition of the new computing trait by transfering ownership
    /// traits.addTrait(std::move(computingTrait));
    /// \endcode
    ///
    void addTrait(std::unique_ptr<ComputingTrait> _trait);

    // This getter is not used anymore. Please use the iterator
    //const std::vector<std::unique_ptr<ComputingTrait> > &getTraits() const;

    ///
    /// \brief Traits Definition of a type for storing the ComputingTraits
    ///
    typedef std::vector<std::unique_ptr<ComputingTrait> > Traits;

    ///
    /// \brief Iterator Definition of an iterator for the ComputingTrait objects
    ///
    typedef Traits::const_iterator Iterator;

    ///
    /// \brief begin returns an iterator on the beginning of the list
    /// \return The iterator on the beginning of the list
    ///
    Iterator begin() const { return m_traits.begin(); }

    ///
    /// \brief end returns an iterator on the end of the list
    /// \return The iterator on the end of the list
    ///
    Iterator end() const { return m_traits.end(); }

protected:

    //! A vector of ComputingTraits
    Traits m_traits;
};

} // namespace Core
} // namespace Tucuxi


#endif // COMPUTINGTRAIT_H
