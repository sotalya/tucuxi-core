/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_DRUGMODELDOMAINCONSTRAINTSEVALUATOR_H
#define TEST_DRUGMODELDOMAINCONSTRAINTSEVALUATOR_H

#include "fructose/fructose.h"

#include "tucucommon/utils.h"
#include "tucucommon/iterator.h"
#include "tucucommon/timeofday.h"
#include "tucucommon/translatablestring.h"

#include "tucucore/covariateextractor.h"
#include "tucucore/parametersextractor.h"

#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugdomainconstraintsevaluator.h"

#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugmodel/covariatedefinition.h"

#include "testutils.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Common::Utils;


/// \brief Add a constraint to a drug model domain.
///        The operation has 2 inputs whose names are specified as parameters.
/// \param DOMAIN DrugModelDomain in which the constraint will be added.
/// \param OPERATION Operation performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param TYPE Type of the constraint (hard or soft).
#define ADD_OP2_CONSTRAINT(DOMAIN, OPERATION, OP1, OP2, TYPE) \
    do { \
    Operation *op = \
    new JSOperation(OPERATION, { \
    OperationInput(OP1, InputType::DOUBLE), \
    OperationInput(OP2, InputType::DOUBLE)}); \
    Constraint *c = new Constraint(); \
    c->addRequiredCovariateId(OP1); \
    c->addRequiredCovariateId(OP2); \
    c->setType(TYPE); \
    c->setCheckOperation(std::unique_ptr<Operation>(op)); \
    DOMAIN.addConstraint(std::unique_ptr<Constraint>(c)); \
    } while (0);

/// \brief Add a constraint to a drug model domain.
///        The operation has 2 inputs whose names are specified as parameters.
/// \param DOMAIN DrugModelDomain pointer in which the constraint will be added.
/// \param OPERATION Operation performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param TYPE Type of the constraint (hard or soft).
#define ADD_OP_CONSTRAINT(DOMAIN, OPERATION, OP1, OP2, TYPE) \
    do { \
    Operation *op = \
    new JSOperation(OPERATION, { \
    OperationInput(OP1, InputType::DOUBLE), \
    OperationInput(OP2, InputType::DOUBLE)}); \
    Constraint *c = new Constraint(); \
    c->addRequiredCovariateId(OP1); \
    c->addRequiredCovariateId(OP2); \
    c->setType(TYPE); \
    c->setCheckOperation(std::unique_ptr<Operation>(op)); \
    DOMAIN->addConstraint(std::unique_ptr<Constraint>(c)); \
    } while (0);

#define ADD_OP1_CONSTRAINT(DOMAIN, OPERATION, OP1, TYPE) \
    do { \
    Operation *op = \
    new JSOperation(OPERATION, { \
    OperationInput(OP1, InputType::DOUBLE)}); \
    Constraint *c = new Constraint(); \
    c->addRequiredCovariateId(OP1); \
    c->setType(TYPE); \
    c->setCheckOperation(std::unique_ptr<Operation>(op)); \
    DOMAIN.addConstraint(std::unique_ptr<Constraint>(c)); \
    } while (0);

void compatibleTests();

struct TestDrugDomainConstraintsEvaluator : public fructose::test_base<TestDrugDomainConstraintsEvaluator>
{
    TestDrugDomainConstraintsEvaluator() {}


    /// \brief Test the extraction capabilities of the Parameters extractor according to test 1_1.
    void test1(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        // Covariates of interest.
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, cm, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Age, 50, AgeInYears, Int, Linear, year, cDefinitions);


        // Set of covariate events.
        CovariateSeries cSeries;
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        cSeries.push_back(CovariateEvent(*(cDefinitions[3]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 10, 0), 51));
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));


        DrugModelDomain domain;

        // Computed parameters
        ADD_OP2_CONSTRAINT(domain, "return Gist;", "Gist", "Weight", ConstraintType::HARD);
        ADD_OP2_CONSTRAINT(domain, "return Weight < 100;", "Weight", "Gist", ConstraintType::HARD);



        DrugDomainConstraintsEvaluator evaluator;

        DrugDomainConstraintsEvaluator::Result rc;

        std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

        rc = evaluator.evaluate(domain, cSeries,
                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                DATE_TIME_NO_VAR(2018, 8, 17, 8, 0, 0),
                                evaluationResults);

        fructose_assert(rc == DrugDomainConstraintsEvaluator::Result::Incompatible);

    }



    /// \brief Test the extraction capabilities of the Parameters extractor according to test 1_1.
    void test2(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        // Covariates of interest.
        ADD_CDEF_NO_R(Gist, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, cm, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Age, 3, AgeInWeeks, Int, Linear, week, cDefinitions);


        // Set of covariate events.
        CovariateSeries cSeries;
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));
        cSeries.push_back(CovariateEvent(*(cDefinitions[3]), DATE_TIME_NO_VAR(2017, 8, 17, 9, 0, 0), 4));


        DrugModelDomain domain;

        // Computed parameters
        ADD_OP2_CONSTRAINT(domain, "return Gist;", "Gist", "Weight", ConstraintType::HARD);
        ADD_OP2_CONSTRAINT(domain, "return Weight < 100;", "Weight", "Gist", ConstraintType::HARD);



        DrugDomainConstraintsEvaluator evaluator;

        DrugDomainConstraintsEvaluator::Result rc;

        std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

        rc = evaluator.evaluate(domain, cSeries,
                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
                                evaluationResults);

        fructose_assert(rc == DrugDomainConstraintsEvaluator::Result::Compatible);

    }




    /// \brief Test the extraction capabilities of the Parameters extractor according to test 1_1.
    void test3(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        // Covariates of interest.
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, cm, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Age, 3, AgeInDays, Int, Linear, week, cDefinitions);

        // Set of covariate events.
        CovariateSeries cSeries;
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));
        cSeries.push_back(CovariateEvent(*(cDefinitions[3]), DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0), 4));


        DrugModelDomain domain;

        // Computed parameters
        ADD_OP2_CONSTRAINT(domain, "return Gist;", "Gist", "Weight", ConstraintType::SOFT);
        ADD_OP2_CONSTRAINT(domain, "return Weight < 100;", "Weight", "Gist", ConstraintType::HARD);



        DrugDomainConstraintsEvaluator evaluator;

        DrugDomainConstraintsEvaluator::Result rc;

        std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

        rc = evaluator.evaluate(domain, cSeries,
                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
                                evaluationResults);

        fructose_assert(rc == DrugDomainConstraintsEvaluator::Result::PartiallyCompatible);

    }


    /// \brief Test the extraction capabilities of the Parameters extractor according to test 1_1.
    void test4(const std::string& /* _testName */)
    {

        compatibleTests(ConstraintType::SOFT, ConstraintType::SOFT);
        compatibleTests(ConstraintType::SOFT, ConstraintType::HARD);
        compatibleTests(ConstraintType::HARD, ConstraintType::SOFT);
        compatibleTests(ConstraintType::HARD, ConstraintType::HARD);

    }

private:

    void compatibleTests(ConstraintType firstConstraintType, ConstraintType secondConstraintType)
    {

        std::unique_ptr<DrugModelDomain> domain = std::make_unique<DrugModelDomain>();

        ADD_OP_CONSTRAINT(domain, "return Gist;", "Gist", "Weight", firstConstraintType);
        ADD_OP_CONSTRAINT(domain, "return Weight < 100;", "Weight", "Gist", secondConstraintType);

        CovariateDefinitions cDefinitions;

        ADD_CDEF_NO_R(Gist, true, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, cm, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Age, 3, AgeInDays, Int, Linear, days, cDefinitions);

        DrugModel drugModel;

        for (auto &cDefinition : cDefinitions)
        {
            drugModel.addCovariate(move(cDefinition));
        }

        drugModel.setDomain(std::move(domain));

        std::unique_ptr<PatientCovariate> covariate1 = std::make_unique<PatientCovariate>("Height", "180", DataType::Double, Unit("cm"), DATE_TIME_NO_VAR(2020, 6, 10, 8, 0, 0));
        std::unique_ptr<PatientCovariate> covariate2 = std::make_unique<PatientCovariate>("Height", "180", DataType::Double, Unit("cm"), DATE_TIME_NO_VAR(2020, 6, 10, 8, 0, 0));
        std::unique_ptr<PatientCovariate> covariate3 = std::make_unique<PatientCovariate>("Weight", "55", DataType::Double, Unit("kg"), DATE_TIME_NO_VAR(2020, 6, 11, 8, 0, 0));
        std::unique_ptr<PatientCovariate> covariate4 = std::make_unique<PatientCovariate>("Sex", "0", DataType::Bool, Unit("-"), DATE_TIME_NO_VAR(2020, 6, 12, 8, 0, 0));

        DrugTreatment drugTreatment;

        drugTreatment.addCovariate(std::unique_ptr<PatientCovariate>(std::move(covariate1)));
        drugTreatment.addCovariate(std::unique_ptr<PatientCovariate>(std::move(covariate2)));
        drugTreatment.addCovariate(std::unique_ptr<PatientCovariate>(std::move(covariate3)));
        drugTreatment.addCovariate(std::unique_ptr<PatientCovariate>(std::move(covariate4)));

        DrugDomainConstraintsEvaluator evaluator;

        DrugDomainConstraintsEvaluator::Result rc;

        std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

        rc = evaluator.evaluate(drugModel, drugTreatment,
                                DATE_TIME_NO_VAR(2020, 6, 8, 8, 0, 0),
                                DATE_TIME_NO_VAR(2020, 6, 13, 8, 0, 0),
                                evaluationResults);

        for(const auto evalResult : evaluationResults)
        {
            fructose_assert(evalResult.m_result == DrugDomainConstraintsEvaluator::Result::Compatible);
        }

        fructose_assert(rc == DrugDomainConstraintsEvaluator::Result::Compatible);
    }

};

#endif // TEST_DRUGMODELDOMAINCONSTRAINTSEVALUATOR_H
