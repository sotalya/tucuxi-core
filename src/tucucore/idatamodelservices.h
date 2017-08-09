/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_IDATAMODELSERVICES_H
#define TUCUXI_CORE_IDATAMODELSERVICES_H

#include "tucucommon/interface.h"

namespace Tucuxi {
namespace Core {

class IDataModelServices : public Tucuxi::Common::Interface
{
public:
    virtual bool loadDrug() = 0;
    virtual bool loadTreatment() = 0;
};

}
}

#endif // TUCUXI_CORE_IDATAMODELSERVICES_H