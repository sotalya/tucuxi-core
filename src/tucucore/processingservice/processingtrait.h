/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef PROCESSINGTRAIT_H
#define PROCESSINGTRAIT_H


#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/processingservice/iprocessingservice.h"


namespace Tucuxi {
namespace Core {

enum class PredictionParameterType { Population, Apiori, Aposteriori };
class CoreComponent;

///
/// \brief The ProcessingTrait class
/// This is the base class for all traits. It only has an identifier.
///
class ProcessingTrait
{
public:
    ///
    /// \brief get the Id of the ProcessingTrait
    /// \return The Id
    ///
    RequestResponseId getId() const;
    
protected:
    ///
    /// \brief ProcessingTrait constructor
    /// \param _id Id of the ProcessingTrait
    /// The constructor is protected, as this class shall not be instanciated.
    /// Only subclasses are relevant.
    ProcessingTrait(RequestResponseId _id);

    RequestResponseId m_id;

private:
    friend class CoreComponent;
    virtual ProcessingResult compute(CoreComponent &_coreComponent) const = 0;    
};

/// This shall be better thought. I think there are more options such as:
/// compute parameter values or not
enum class ProcessingOption {
    MainCompartment = 0,
    AllCompartments
};

///
/// \brief The ProcessingTraitStandard class
/// This is a base class for other Traits. It embeds information about:
/// 1. Type of parameters
/// 2. Start date of prediction calculation
/// 3. End date of prediction calculation
/// 4. Number of points for the calculation
/// 5. Some processing options
/// It shall not be instanciated, and this fact is assured by the protected constructor.
///
class ProcessingTraitStandard : public ProcessingTrait
{
public:
    PredictionParameterType getType() const;
    ProcessingOption getProcessingOption() const;
    const Tucuxi::Common::DateTime& getStart() const;
    const Tucuxi::Common::DateTime& getEnd() const;
    int getNbPoints() const;

protected:
    ProcessingTraitStandard(RequestResponseId _id,
                            PredictionParameterType _type,
                            Tucuxi::Common::DateTime _start,
                            Tucuxi::Common::DateTime _end,
                            int _nbPoints,
                            ProcessingOption _processingOption);

private:
    PredictionParameterType m_type;
    ProcessingOption m_processingOption;
    Tucuxi::Common::DateTime m_start;
    Tucuxi::Common::DateTime m_end;
    int m_nbPoints;
};

///
/// \brief The ProcessingTraitSinglePoints class
/// This class embeds the information required to compute concentrations at
/// specific time points.
///
class ProcessingTraitSinglePoints : public ProcessingTrait
{
public:
    ProcessingTraitSinglePoints(RequestResponseId _id,
                                std::vector<Tucuxi::Common::DateTime> _times,
                                ProcessingOption _processingOption);

protected:
    std::vector<Tucuxi::Common::DateTime> m_times;
    PredictionParameterType m_type;
    ProcessingOption m_processingOption;

private:
    ProcessingResult compute(CoreComponent &_coreComponent) const override { return ProcessingResult::Error; }
};

///
/// \brief The ProcessingTraitAtMeasures class
/// This class shall be used for computing concentrations at the times of
/// the DrugTreatment measures. Typically used for comparing the measured
/// concentrations with a posteriori predictions.
///
class ProcessingTraitAtMeasures : public ProcessingTrait
{
public:
    ProcessingTraitAtMeasures(RequestResponseId _id, ProcessingOption _processingOption);

protected:
    PredictionParameterType m_type;
    ProcessingOption m_processingOption;

private:
    ProcessingResult compute(CoreComponent &_coreComponent) const override { return ProcessingResult::Error; }
};

///
/// \brief The AdjustmentOption enum
/// This option allow to ask for a single dosage adjustement or for all acceptable adjustments
enum class AdjustmentOption
{
    /// Only return the best dosage
    BestDosage = 0,

    /// Return all acceptable dosages
    AllDosages
};

///
/// \brief The ProcessingTraitAdjustment class
/// This class embeds all information required for computing adjustments. It can return
/// potential dosages, and future concentration calculations, depending on the options.
///
/// If nbPoints = 0, then no curve will be returned by the computation, only the dosages
///
class ProcessingTraitAdjustment : public ProcessingTraitStandard
{
public:
    ///
    /// \brief ProcessingTraitAdjustment
    /// \param _id
    /// \param _type
    /// \param _start
    /// \param _end
    /// \param _nbPoints
    /// \param _processingOption
    /// \param _adjustmentTime
    /// \param _adjustmentOption
    /// If nbPoints = 0, then no curve will be returned by the computation, only the dosages
    ProcessingTraitAdjustment(RequestResponseId _id,
                              PredictionParameterType _type,
                              Tucuxi::Common::DateTime _start,
                              Tucuxi::Common::DateTime _end,
                              int _nbPoints,
                              ProcessingOption _processingOption,
                              Tucuxi::Common::DateTime _adjustmentTime,
                              AdjustmentOption _adjustmentOption);

    const Tucuxi::Common::DateTime& getAdjustmentTime() const;
    AdjustmentOption getAdjustmentOption() const;

protected:
    Tucuxi::Common::DateTime& m_adjustmentTime;
    AdjustmentOption m_adjustmentOption;

private:
    ProcessingResult compute(CoreComponent &_coreComponent) const override { return ProcessingResult::Error; }
};

///
/// \brief The ProcessingTraitConcentration class
/// This class represents a request for a single prediction
///
class ProcessingTraitConcentration : public ProcessingTraitStandard
{
public:
    ProcessingTraitConcentration(RequestResponseId _id,
                                 PredictionParameterType _type,
                                 Tucuxi::Common::DateTime _start,
                                 Tucuxi::Common::DateTime _end,
                                 int _nbPoints,
                                 ProcessingOption _processingOption);
private:
    ProcessingResult compute(CoreComponent &_coreComponent) const override;
};


///
/// \brief The ProcessingTraitPercentiles class
/// This class embeds all information for calculating percentiles. Additionnaly to
/// standard single predictions, it requires the list of asked percentiles.
/// This list is a vector of values in [0.0, 100.0]
class ProcessingTraitPercentiles : public ProcessingTraitStandard
{
public:
    ProcessingTraitPercentiles(RequestResponseId _id,
                               PredictionParameterType _type,
                               Tucuxi::Common::DateTime _start,
                               Tucuxi::Common::DateTime _end,
                               const PercentileRanks &_ranks,
                               int _nbPoints,
                               ProcessingOption _processingOption);

private:
    PercentileRanks m_ranks;

private:
    ProcessingResult compute(CoreComponent &_coreComponent) const override { return ProcessingResult::Error; }
};


///
/// \brief The ProcessingTraits class
/// This class is a collection of ProcessingTrait. It is meant to group various
/// calculations that will be applied on a single DrugTreatment and a single
/// DrugModel.
/// For instance, if a client needs a prediction, percentiles, and a proposition
/// of dosage adjustment, then three ProcessingTrait will be added to a ProcessingTraits
class ProcessingTraits
{
public:
    void addTrait(std::unique_ptr<ProcessingTrait> _trait);
    //const std::vector<std::unique_ptr<ProcessingTrait> > &getTraits() const;

    typedef std::vector<std::unique_ptr<ProcessingTrait> > Traits;
    typedef Traits::const_iterator Iterator;
    Iterator begin() const { return m_traits.begin(); };
    Iterator end() const { return m_traits.end(); };

protected:
    Traits m_traits;
};

}
}


#endif // PROCESSINGTRAIT_H
