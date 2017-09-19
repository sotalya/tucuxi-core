/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef PROCESSINGTRAIT_H
#define PROCESSINGTRAIT_H


#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugmodel/drugmodel.h"


namespace Tucuxi {
namespace Core {

enum class PredictionParameterType { Population, Apiori, Aposteriori };


class ProcessingTrait
{
public:
    ProcessingTrait(std::string _id) : m_id(_id) {}
    std::string getId() const { return m_id;}

protected:
    std::string m_id;
};

enum class ProcessingOption {
    MainCompartment = 0,
    AllCompartments
};

class ProcessingTraitStandard : public ProcessingTrait
{

public:
    PredictionParameterType getType() const { return m_type; }
    ProcessingOption getProcessingOption() const { return m_processingOption; }
    const Tucuxi::Common::DateTime& getStart() const { return m_start; }
    const Tucuxi::Common::DateTime& getEnd() const { return m_end; }
    int getNbPoints() const { return m_nbPoints; }

protected:
    ProcessingTraitStandard(std::string _id,
                            PredictionParameterType _type,
                            Tucuxi::Common::DateTime _start,
                            Tucuxi::Common::DateTime _end,
                            int _nbPoints,
                            ProcessingOption _processingOption) :
        ProcessingTrait(_id),
        m_type(_type), m_processingOption(_processingOption), m_start(_start), m_end(_end), m_nbPoints(_nbPoints) {}

private:
    PredictionParameterType m_type;
    ProcessingOption m_processingOption;
    Tucuxi::Common::DateTime m_start;
    Tucuxi::Common::DateTime m_end;
    int m_nbPoints;
};


enum class AdjustmentOption
{
    BestDosage = 0,
    AllDosages
};

///
/// \brief The ProcessingTraitAdjustment class
/// If nbPoints = 0, then no curve will be returned by the computation, only the dosages
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
    ProcessingTraitAdjustment(std::string _id,
                              PredictionParameterType _type,
                              Tucuxi::Common::DateTime _start,
                              Tucuxi::Common::DateTime _end,
                              int _nbPoints,
                              ProcessingOption _processingOption,
                              Tucuxi::Common::DateTime _adjustmentTime,
                              AdjustmentOption _adjustmentOption) :
        ProcessingTraitStandard(_id, _type, _start, _end, _nbPoints, _processingOption),
        m_adjustmentTime(_adjustmentTime),
        m_adjustmentOption(_adjustmentOption){}

    const Tucuxi::Common::DateTime& getAdjustmentTime() const { return m_adjustmentTime;}
    AdjustmentOption getAdjustmentOption() const { return m_adjustmentOption;}

protected:
    Tucuxi::Common::DateTime& m_adjustmentTime;
    AdjustmentOption m_adjustmentOption;
};

class ProcessingTraitConcentration : public ProcessingTraitStandard
{
    ProcessingTraitConcentration(std::string _id,
                                 PredictionParameterType _type,
                                 Tucuxi::Common::DateTime _start,
                                 Tucuxi::Common::DateTime _end,
                                 int _nbPoints,
                                 ProcessingOption _processingOption) :
        ProcessingTraitStandard(_id, _type, _start, _end, _nbPoints, _processingOption) {}

};

/// Shall we allow doubles? I would say yes
typedef double PercentileRank;
typedef std::vector<PercentileRank> PercentileRanks;

class ProcessingTraitPercentiles : public ProcessingTraitStandard
{

    ProcessingTraitPercentiles(std::string _id,
                               PredictionParameterType _type,
                               Tucuxi::Common::DateTime _start,
                               Tucuxi::Common::DateTime _end,
                               const PercentileRanks &_ranks,
                               int _nbPoints,
                               ProcessingOption _processingOption) :
        ProcessingTraitStandard(_id, _type, _start, _end, _nbPoints, _processingOption), m_ranks(_ranks) {}

private:

    PercentileRanks m_ranks;
};



class ProcessingTraits
{
public:

    void addTrait(std::unique_ptr<ProcessingTrait> _trait);
    const std::vector<std::unique_ptr<ProcessingTrait> > &getTraits() const;

protected:

    std::vector<std::unique_ptr<ProcessingTrait> > m_traits;
};

}
}


#endif // PROCESSINGTRAIT_H
