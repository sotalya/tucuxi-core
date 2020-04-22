#include "residualerrormodelextractor.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/drugmodel/errormodel.h"

namespace Tucuxi {
namespace Core {

ResidualErrorModelExtractor::ResidualErrorModelExtractor()
{

}


ComputingStatus ResidualErrorModelExtractor::extract(
        const ErrorModel &_errorModel,
        const Unit &_fromUnit,
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
    Unit toUnit("ug/l");

    Sigma sigma = Sigma(_errorModel.m_originalSigmas.size());
    for(std::size_t i = 0;i < _errorModel.m_originalSigmas.size(); i++) {
        if ((_errorModel.m_errorModel == ResidualErrorType::MIXED) && (i == 0)) {
            sigma[i] = translateToUnit(_errorModel.m_originalSigmas[i]->getValue(), _fromUnit, toUnit);
        }
        else if (_errorModel.m_errorModel == ResidualErrorType::ADDITIVE) {
            sigma[i] = translateToUnit(_errorModel.m_originalSigmas[i]->getValue(), _fromUnit, toUnit);
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
