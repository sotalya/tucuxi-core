/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef COMPUTINGTRAIT_H
#define COMPUTINGTRAIT_H


#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/computingservice/icomputingservice.h"


namespace Tucuxi {
namespace Core {

enum class PredictionParameterType { Population, Apriori, Aposteriori };
class CoreComponent;

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
    
protected:
    ///
    /// \brief ComputingTrait constructor
    /// \param _id Id of the ComputingTrait
    /// The constructor is protected, as this class shall not be instanciated.
    /// Only subclasses are relevant.
    ComputingTrait(RequestResponseId _id);

    RequestResponseId m_id;

private:
    friend class CoreComponent;
    virtual ComputingResult compute(CoreComponent &_coreComponent) const = 0;
};

/// This shall be better thought. I think there are more options such as:
/// compute parameter values or not
enum class ComputingOption {
    MainCompartment = 0,
    AllCompartments
};

///
/// \brief The ComputingTraitStandard class
/// This is a base class for other Traits. It embeds information about:
/// 1. Type of parameters
/// 2. Start date of prediction calculation
/// 3. End date of prediction calculation
/// 4. Number of points for the calculation
/// 5. Some processing options
/// It shall not be instanciated, and this fact is assured by the protected constructor.
///
class ComputingTraitStandard : public ComputingTrait
{
public:
    PredictionParameterType getType() const;
    ComputingOption getComputingOption() const;
    const Tucuxi::Common::DateTime& getStart() const;
    const Tucuxi::Common::DateTime& getEnd() const;
    CycleSize getCycleSize() const;

protected:
    ComputingTraitStandard(RequestResponseId _id,
                            PredictionParameterType _type,
                            Tucuxi::Common::DateTime _start,
                            Tucuxi::Common::DateTime _end,
                            const CycleSize _cycleSize,
                            ComputingOption _computingOption);

private:
    PredictionParameterType m_type;
    ComputingOption m_computingOption;
    Tucuxi::Common::DateTime m_start;
    Tucuxi::Common::DateTime m_end;
    CycleSize m_cycleSize;
};

///
/// \brief The ComputingTraitSinglePoints class
/// This class embeds the information required to compute concentrations at
/// specific time points.
///
class ComputingTraitSinglePoints : public ComputingTrait
{
public:
    ComputingTraitSinglePoints(RequestResponseId _id,
                                std::vector<Tucuxi::Common::DateTime> _times,
                                ComputingOption _computingOption);

protected:
    std::vector<Tucuxi::Common::DateTime> m_times;
    PredictionParameterType m_type;
    ComputingOption m_computingOption;

private:
    ComputingResult compute(CoreComponent &_coreComponent) const override { return ComputingResult::Error; }
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
    ComputingTraitAtMeasures(RequestResponseId _id, ComputingOption _computingOption);

protected:
    PredictionParameterType m_type;
    ComputingOption m_computingOption;

private:
    ComputingResult compute(CoreComponent &_coreComponent) const override { return ComputingResult::Error; }
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
    /// \param _type
    /// \param _start
    /// \param _end
    /// \param _nbPoints
    /// \param _computingOption
    /// \param _adjustmentTime
    /// \param _adjustmentOption
    /// If nbPoints = 0, then no curve will be returned by the computation, only the dosages
    ComputingTraitAdjustment(RequestResponseId _id,
                              PredictionParameterType _type,
                              Tucuxi::Common::DateTime _start,
                              Tucuxi::Common::DateTime _end,
                              const CycleSize _cycleSize,
                              ComputingOption _computingOption,
                              Tucuxi::Common::DateTime _adjustmentTime,
                              AdjustmentOption _adjustmentOption);

    const Tucuxi::Common::DateTime& getAdjustmentTime() const;
    AdjustmentOption getAdjustmentOption() const;

protected:
    Tucuxi::Common::DateTime& m_adjustmentTime;
    AdjustmentOption m_adjustmentOption;

private:
    ComputingResult compute(CoreComponent &_coreComponent) const override { return ComputingResult::Error; }
};

///
/// \brief The ComputingTraitConcentration class
/// This class represents a request for a single prediction
///
class ComputingTraitConcentration : public ComputingTraitStandard
{
public:
    ComputingTraitConcentration(RequestResponseId _id,
                                 PredictionParameterType _type,
                                 Tucuxi::Common::DateTime _start,
                                 Tucuxi::Common::DateTime _end,
                                 const CycleSize _cycleSize,
                                 ComputingOption _computingOption);
private:
    ComputingResult compute(CoreComponent &_coreComponent) const override;
};


///
/// \brief The ComputingTraitPercentiles class
/// This class embeds all information for calculating percentiles. Additionnaly to
/// standard single predictions, it requires the list of asked percentiles.
/// This list is a vector of values in [0.0, 100.0]
class ComputingTraitPercentiles : public ComputingTraitStandard
{
public:
    ComputingTraitPercentiles(RequestResponseId _id,
                               PredictionParameterType _type,
                               Tucuxi::Common::DateTime _start,
                               Tucuxi::Common::DateTime _end,
                               const PercentileRanks &_ranks,
                               const CycleSize _cycleSize,
                               ComputingOption _computingOption);

private:
    PercentileRanks m_ranks;

private:
    ComputingResult compute(CoreComponent &_coreComponent) const override { return ComputingResult::Error; }
};


///
/// \brief The ComputingTraits class
/// This class is a collection of ComputingTrait. It is meant to group various
/// calculations that will be applied on a single DrugTreatment and a single
/// DrugModel.
/// For instance, if a client needs a prediction, percentiles, and a proposition
/// of dosage adjustment, then three ComputingTrait will be added to a ComputingTraits
class ComputingTraits
{
public:
    void addTrait(std::unique_ptr<ComputingTrait> _trait);
    //const std::vector<std::unique_ptr<ComputingTrait> > &getTraits() const;

    typedef std::vector<std::unique_ptr<ComputingTrait> > Traits;
    typedef Traits::const_iterator Iterator;
    Iterator begin() const { return m_traits.begin(); };
    Iterator end() const { return m_traits.end(); };

protected:
    Traits m_traits;
};

}
}


#endif // COMPUTINGTRAIT_H
