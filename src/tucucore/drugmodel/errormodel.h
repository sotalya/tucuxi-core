#ifndef TUCUXI_CORE_ERRORMODEL_H
#define TUCUXI_CORE_ERRORMODEL_H

#include <memory>

#include "tucucore/iresidualerrormodel.h"
#include "tucucore/invariants.h"

namespace Tucuxi {
namespace Core {

class ResidualErrorModelExtractor;

class ErrorModel
{
public:
    ErrorModel();

    void setApplyFormula(std::unique_ptr<Operation> _applyFormula) {m_applyFormula = std::move(_applyFormula);}
    void setLikelyhoodFormula(std::unique_ptr<Operation> _likelyhoodFormula) {m_likelyhoodFormula = std::move(_likelyhoodFormula);}

    void addOriginalSigma(std::unique_ptr<PopulationValue> _sigma) {m_originalSigmas.push_back(std::move(_sigma));}
    void setErrorModel(ResidualErrorType _errorModel) { m_errorModel = _errorModel;}


    INVARIANTS(
            // No invariants
            )

protected:

    std::unique_ptr<Operation> m_applyFormula;
    std::unique_ptr<Operation> m_likelyhoodFormula;

    std::vector<std::unique_ptr<PopulationValue> > m_originalSigmas;

    /// The residual error model type
    ResidualErrorType m_errorModel;

    friend ResidualErrorModelExtractor;
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_ERRORMODEL_H
