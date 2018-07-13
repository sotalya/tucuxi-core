/*
* Copyright (C) 2017 Tucuxi SA
*/
 
#ifndef TUCUXI_CORE_IPROCESSINGSERVICES_H
#define TUCUXI_CORE_IPROCESSINGSERVICES_H
 
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
 
#include "tucucommon/interface.h"
#include "tucucommon/datetime.h"
#include "tucucommon/general.h"
 
#include "tucucore/definitions.h"
 
namespace Tucuxi {
namespace Core {
/* 
enum class PredictionType { Population, Apiori, Aposteriori };
 
class PredictionRequest 
{    
public:
    PredictionType getType() const { return m_type; }
    bool getIsAll() const { return m_isAll; }
    const Tucuxi::Common::DateTime& getStart() const { return m_start; }
    const Tucuxi::Common::DateTime& getEnd() const { return m_end; }
    int getNbPoints() const { return m_nbPoints; }
 
protected:
    PredictionRequest() {}
    PredictionRequest(PredictionType _type,
	bool _isAll,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        int _nbPoints) :
        m_type(_type), m_isAll(_isAll), m_start(_start), m_end(_end), m_nbPoints(_nbPoints) {}
 
private:
    PredictionType m_type;
    bool m_isAll;
    Tucuxi::Common::DateTime m_start;
    Tucuxi::Common::DateTime m_end;
    int m_nbPoints;
};
 
class ConcentrationRequest : public PredictionRequest
{
public:
    ConcentrationRequest() = delete;
    ConcentrationRequest(const std::string& _xmlRequest) { TMP_UNUSED_PARAMETER(_xmlRequest);}
    ConcentrationRequest(PredictionType _type,
	bool _isAll,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        int _nbPoints) :
        PredictionRequest(_type, _isAll, _start, _end, _nbPoints) {}
};

typedef Value Percentile;
typedef std::vector<Percentile> Percentiles;
 
class PercentilesRequest : public PredictionRequest
{
public:
    PercentilesRequest() = delete;
    PercentilesRequest(const std::string& _xmlRequest) { TMP_UNUSED_PARAMETER(_xmlRequest);}
    PercentilesRequest(PredictionType _type,
	bool _isAll,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        const PercentileRanks &_ranks,
        int _nbPoints) :
        PredictionRequest(_type, _isAll, _start, _end, _nbPoints), m_ranks(_ranks) {}
private:
    PercentileRanks m_ranks;
};
*/ 


 
/*
class IComputingServices : public Tucuxi::Common::Interface
{
public:
    virtual ConcentrationPredictionPtr computeConcentrations(const ConcentrationRequest& request) = 0;
    virtual PercentilesPredictionPtr computePercentiles(const PercentilesRequest& request) = 0;
    virtual void computeAdjustments() = 0;
};
*/
 
}
}
 
#endif // TUCUXI_CORE_IPROCESSINGSERVICES_H
