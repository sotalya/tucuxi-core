#include "computingutils.h"

#include "tucucore/operation.h"
#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/generalextractor.h"

namespace Tucuxi {
namespace Core {

ComputingUtils::ComputingUtils()
= default;


ComputingStatus ComputingUtils::computeActiveMoiety(
        const ActiveMoiety *_activeMoiety,
        const std::vector<ConcentrationPredictionPtr> &_analytesPredictions,
        ConcentrationPredictionPtr &_activeMoietyPredictions)
{

    Operation *op = _activeMoiety->getFormula();

    size_t fullSize = _analytesPredictions[0]->getValues().size();

    size_t nbAnalytes = _analytesPredictions.size();

    Concentrations concentration;
    for (size_t i = 0; i < fullSize; i++) {
        TimeOffsets times = _analytesPredictions[0]->getTimes()[i];
        std::vector<Concentrations> analyteC(nbAnalytes);
        for(size_t an = 0; an < nbAnalytes; an ++) {
            analyteC[an] = _analytesPredictions[an]->getValues()[i];
        }

        size_t nbConcentrations = analyteC[0].size();
        Concentrations concentration(nbConcentrations);

        if (op == nullptr) {
            for(size_t c = 0; c < nbConcentrations; c++) {
                concentration[c] = analyteC[0][c];
            }
        }
        else {

            OperationInputList inputList;
            for (size_t an = 0; an < nbAnalytes; an++) {
                inputList.push_back(OperationInput("input" + std::to_string(an), 0.0));
            }

            for(size_t c = 0; c < nbConcentrations; c++) {
                for (size_t an = 0; an < nbAnalytes; an++) {
                    inputList[an].setValue(analyteC[an][c]);
                }
                double result;
                if (!op->evaluate(inputList, result)) {
                    // Something went wrong
                    return ComputingStatus::ActiveMoietyCalculationError;
                }
                concentration[c] = result;
            }
        }
        _activeMoietyPredictions->appendConcentrations(times, concentration);
    }

    return ComputingStatus::Ok;
}


} // namespace Core
} // namespace Tucuxi
