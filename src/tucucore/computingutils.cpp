#include "computingutils.h"

#include "tucucore/operation.h"
#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/generalextractor.h"
#include "multiconcentrationprediction.h"

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






CovariateEvent ComputingUtils::getCovariateAtTime(const DateTime &_date, const CovariateSeries &_covariates)
{
    // Find the lasted change that occured before the given date
    std::vector<CovariateEvent>::const_iterator it = _covariates.begin();
    if (it != _covariates.end()) {
        std::vector<CovariateEvent>::const_iterator itNext = it;
        while (++itNext != _covariates.end() && _date > itNext->getEventTime()) {
            it++;
        }
    }

    // Did we find something?
    if (it != _covariates.end())
    {
        // Make a copy to hold values with applied etas
        return CovariateEvent(*it);
    }

    // Should not happen
    std::cout << "Something bad is currently happening" << std::endl;
    return CovariateEvent(*it);
}



ComputingStatus ComputingUtils::computeMultiActiveMoiety(
        const ActiveMoiety *_activeMoiety0,
        const ActiveMoiety *_activeMoiety1,
        const std::vector<MultiConcentrationPredictionPtr> &_analytesPredictions,
        MultiConcentrationPredictionPtr &_activeMoietyPredictions)
{
    Operation *op1 = _activeMoiety1->getFormula();
    Operation *op2 = _activeMoiety0->getFormula();

    size_t fullSize = _analytesPredictions[0]->getValues().size();

    size_t nbAnalytes = _analytesPredictions.size();

    Concentrations concentration;
    for (size_t i = 0; i < fullSize; i++) {
        TimeOffsets times = _analytesPredictions[0]->getTimes()[i];
        std::vector<Concentrations> analyteC(nbAnalytes);
        for(size_t an = 0; an < nbAnalytes; an ++) {
            for(size_t j = 0; j < 1; j++) analyteC[an] = _analytesPredictions[an]->getValues()[i][j]; //done for 2 active moietys
        }

        size_t nbConcentrations = analyteC[0].size();
        std::vector<Concentrations> concentration(nbConcentrations);

        if (op1 == nullptr) {
            for(size_t c = 0; c < nbConcentrations; c++) {
                concentration[c][0] = analyteC[0][c];
            }
        }

        else if (op2 == nullptr){
            for(size_t c = 0; c < nbConcentrations; c++) {
                concentration[c][1] = analyteC[1][c];
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
                if (!op1->evaluate(inputList, result) || !op2->evaluate(inputList, result)) {
                    // Something went wrong
                    return ComputingStatus::MultiActiveMoietyCalculationError;
                }
                concentration[c][0] = result;
                concentration[c][1] = result;
            }
        }
        _activeMoietyPredictions->appendConcentrations(times, concentration); //i have to put it in function of a vector of concentrations and not a single concentration
    }

    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
