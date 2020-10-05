#include "residualerrormodelextractor.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/drugmodel/errormodel.h"

using Tucuxi::Common::UnitManager;

namespace Tucuxi {
namespace Core {

ResidualErrorModelExtractor::ResidualErrorModelExtractor()
= default;


ComputingStatus ResidualErrorModelExtractor::extract(
        const ErrorModel &_errorModel,
        const TucuUnit &_fromUnit,
        const CovariateSeries &_covariateSeries,
        std::unique_ptr<IResidualErrorModel> &_residualErrorModel)
{
    // At some stage the covariates could influence the error model. Maybe...
    TMP_UNUSED_PARAMETER(_covariateSeries);

    if (_errorModel.m_errorModel == ResidualErrorType::SOFTCODED) {
        // Not supported yet
        return ComputingStatus::ErrorModelExtractionError;
    }

    SigmaResidualErrorModel *newErrorModel = new SigmaResidualErrorModel();
    newErrorModel->setErrorModel(_errorModel.m_errorModel);
    TucuUnit toUnit("ug/l");

    Sigma sigma = Sigma(_errorModel.m_originalSigmas.size());
    for(std::size_t i = 0;i < _errorModel.m_originalSigmas.size(); i++) {
        if ((_errorModel.m_errorModel == ResidualErrorType::MIXED) && (i == 0)) {
            sigma[i] = UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(_errorModel.m_originalSigmas[i]->getValue(), _fromUnit, toUnit);
        }
        else if (_errorModel.m_errorModel == ResidualErrorType::ADDITIVE) {
            sigma[i] = UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(_errorModel.m_originalSigmas[i]->getValue(), _fromUnit, toUnit);
        }
        else {
            sigma[i] = _errorModel.m_originalSigmas[i]->getValue();
        }
    }

    newErrorModel->setSigma(sigma);

    _residualErrorModel = std::unique_ptr<IResidualErrorModel>(newErrorModel);

    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
