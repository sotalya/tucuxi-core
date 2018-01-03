/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "target.h"

#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Core {

Target::Target(const std::string& _analyteId, 
               const TargetType _type, 
               const Value _min, 
               const Value _best, 
               const Value _max)
    : m_analyteId(_analyteId),
      m_targetType(_type),
      m_valueMin(_min),
      m_valueMax(_max),
      m_valueBest(_best)
{
}

Target::Target(const std::string& _analyteId, 
               const TargetType _type,
               const Value _vmin, 
               const Value _vbest, 
               const Value _vmax,
               const Tucuxi::Common::Duration &_tmin, 
               const Tucuxi::Common::Duration &_tbest, 
               const Tucuxi::Common::Duration &_tmax)
    : m_analyteId(_analyteId),
      m_targetType(_type),
      m_valueMin(_vmin),
      m_valueMax(_vmax),
      m_valueBest(_vbest),
      m_tMin(_tmin),
      m_tMax(_tmax),
      m_tBest(_tbest)
{
}

}
}
