/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CORECOMPONENT_H
#define TUCUXI_CORE_CORECOMPONENT_H

#include <memory>

#include "tucucommon/component.h"

//#include "tucucore/icorecomponent.h"
#include "tucucore/iprocessingservices.h"
#include "tucucore/idatamodelservices.h"

namespace Tucuxi {
namespace Core {

class DrugModel;
class DrugTreatment;

class CoreComponent : public Tucuxi::Common::Component, 
//                      public ICoreComponent, 
                      public IProcessingServices,
                      public IDataModelServices
{
public:
    /// \brief Constructor call by LoggerHelper
    ~CoreComponent();

    bool loadDrug() override;
    bool loadTreatment() override;
        

    virtual ConcentrationPredictionPtr computeConcentrations(PredictionType _type, 
                                                             Tucuxi::Common::DateTime _start, 
                                                             Tucuxi::Common::DateTime _end, 
                                                             int _nbPoints) override;

    virtual PercentilesPredictionPtr computePercentiles(PredictionType _type,
                                                        Tucuxi::Common::DateTime _start,
                                                        Tucuxi::Common::DateTime _end,
                                                        const PercentileRanks &_ranks,
                                                        int _nbPoints) override;

    void computeAdjustments() override;

protected:
    /// \brief Access other interfaces of the same component.
    virtual Tucuxi::Common::Interface* getInterface(const std::string &_name);

private:
    /// \brief Constructor call by LoggerHelper
    CoreComponent(const std::string &_filename);

private:
    std::unique_ptr<DrugModel> m_drug;
    std::unique_ptr<DrugTreatment> m_treatment;
};

}
}

#endif // TUCUXI_CORE_CORECOMPONENT_H