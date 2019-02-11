#include "residualerrormodelextractor.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/drugmodel/errormodel.h"

namespace Tucuxi {
namespace Core {

ResidualErrorModelExtractor::ResidualErrorModelExtractor()
{

}


ResidualErrorModelExtractor::Result ResidualErrorModelExtractor::extract(const ErrorModel &_errorModel, const Unit &_fromUnit, const CovariateSeries &_covariateSeries, IResidualErrorModel **_residualErrorModel)
{
    if (_errorModel.m_errorModel == ResidualErrorType::SOFTCODED) {
        // Not supported yet
        return Result::ExtractionError;
    }

    SigmaResidualErrorModel *newErrorModel = new SigmaResidualErrorModel();
    newErrorModel->setErrorModel(_errorModel.m_errorModel);
    Unit toUnit("ug/l");

    Sigma sigma = Sigma(_errorModel.m_originalSigmas.size());
    for(std::size_t i = 0;i < _errorModel.m_originalSigmas.size(); i++) {
        if (_errorModel.m_errorModel == ResidualErrorType::ADDITIVE) {
            sigma[i] = translateToUnit(_errorModel.m_originalSigmas.at(i)->getValue(), _fromUnit, toUnit);
        }
        else {
            sigma[i] = _errorModel.m_originalSigmas.at(i)->getValue();
        }
    }

    *_residualErrorModel = newErrorModel;

    return Result::Ok;
}

} // namespace Core
} // namespace Tucuxi
