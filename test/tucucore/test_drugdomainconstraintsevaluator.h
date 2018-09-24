/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_DRUGMODELDOMAINCONSTRAINTSEVALUATOR_H
#define TEST_DRUGMODELDOMAINCONSTRAINTSEVALUATOR_H

#include "fructose/fructose.h"

#include "tucucommon/utils.h"
#include "tucucommon/iterator.h"
#include "tucucommon/timeofday.h"

#include "tucucore/covariateextractor.h"
#include "tucucore/parametersextractor.h"

#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugdomainconstraintsevaluator.h"

#include "testutils.h"

using namespace Tucuxi::Core;


/// \brief Add a constraint to a drug model domain.
///        The operation has 2 inputs whose names are specified as parameters.
/// \param DOMAIN DrugModelDaomain in which the constraint will be added.
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


struct TestDrugDomainConstraintsEvaluator : public fructose::test_base<TestDrugDomainConstraintsEvaluator>
{
    TestDrugDomainConstraintsEvaluator() { }



    /// \brief Test the extraction capabilities of the Parameters extractor according to test 1_1.
    void test1(const std::string& /* _testName */)
    {
        CovariateDefinitions cDefinitions;
        // Covariates of interest.
        ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
        ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), kg, cDefinitions);
        ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, cm, cDefinitions);

        // Set of covariate events.
        CovariateSeries cSeries;
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(0)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(1)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(2)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(0)), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(2)), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));


        DrugModelDomain domain;

        // Computed parameters
        ADD_OP2_CONSTRAINT(domain, "Gist", "Gist", "Weight", ConstraintType::HARD);
        ADD_OP2_CONSTRAINT(domain, "Weight < 100", "Weight", "Gist", ConstraintType::HARD);



        DrugDomainConstraintsEvaluator evaluator;

        DrugDomainConstraintsEvaluator::Result rc;

        rc = evaluator.evaluate(domain, cSeries,
                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0));

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

        // Set of covariate events.
        CovariateSeries cSeries;
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(0)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(1)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(2)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(0)), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(2)), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));


        DrugModelDomain domain;

        // Computed parameters
        ADD_OP2_CONSTRAINT(domain, "Gist", "Gist", "Weight", ConstraintType::HARD);
        ADD_OP2_CONSTRAINT(domain, "Weight < 100", "Weight", "Gist", ConstraintType::HARD);



        DrugDomainConstraintsEvaluator evaluator;

        DrugDomainConstraintsEvaluator::Result rc;

        rc = evaluator.evaluate(domain, cSeries,
                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0));

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

        // Set of covariate events.
        CovariateSeries cSeries;
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(0)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(false)));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(1)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(2)), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(0)), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(true)));
        cSeries.push_back(CovariateEvent(*(cDefinitions.at(2)), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));


        DrugModelDomain domain;

        // Computed parameters
        ADD_OP2_CONSTRAINT(domain, "Gist", "Gist", "Weight", ConstraintType::SOFT);
        ADD_OP2_CONSTRAINT(domain, "Weight < 100", "Weight", "Gist", ConstraintType::HARD);



        DrugDomainConstraintsEvaluator evaluator;

        DrugDomainConstraintsEvaluator::Result rc;

        rc = evaluator.evaluate(domain, cSeries,
                                DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
                                DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0));

        fructose_assert(rc == DrugDomainConstraintsEvaluator::Result::PartiallyCompatible);

    }

};

#endif // TEST_DRUGMODELDOMAINCONSTRAINTSEVALUATOR_H
