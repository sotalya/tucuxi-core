/*
* Copyright (C) 2017 Tucuxi SA
*/
#include "processingtrait.h"


namespace Tucuxi {
namespace Core {



void ProcessingTraits::addTrait(std::unique_ptr<ProcessingTrait> _trait)
{
    m_traits.push_back(std::move(_trait));
}

const std::vector<std::unique_ptr<ProcessingTrait> > &ProcessingTraits::getTraits() const
{
    return m_traits;
}


}
}
