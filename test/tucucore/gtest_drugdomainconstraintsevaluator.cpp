//@@license@@

#include <gtest/gtest.h>

#include "tucucommon/timeofday.h"
#include "tucucommon/utils.h"

#include "tucucore/drugdomainconstraintsevaluator.h"
#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugtreatment/drugtreatment.h"

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
#define ADD_OP2_CONSTRAINT(DOMAIN, OPERATION, OP1, OP2, TYPE)                                                        \
    do {                                                                                                             \
        auto op = std::make_unique<JSOperation>(                                                                     \
                OPERATION,                                                                                           \
                OperationInputList{OperationInput(OP1, InputType::DOUBLE), OperationInput(OP2, InputType::DOUBLE)}); \
        auto c = std::make_unique<Constraint>();                                                                     \
        c->addRequiredCovariateId(OP1);                                                                              \
        c->addRequiredCovariateId(OP2);                                                                              \
        c->setType(TYPE);                                                                                            \
        c->setCheckOperation(std::move(op));                                                                         \
        DOMAIN.addConstraint(std::move(c));                                                                          \
    } while (0);

/// \brief Add a constraint to a drug model domain.
///        The operation has 2 inputs whose names are specified as parameters.
/// \param DOMAIN DrugModelDomain pointer in which the constraint will be added.
/// \param OPERATION Operation performed to get the values of the computed covariate.
/// \param OP1 Name of the first operand.
/// \param OP2 Name of the second operand.
/// \param TYPE Type of the constraint (hard or soft).
#define ADD_OP_CONSTRAINT(DOMAIN, OPERATION, OP1, OP2, TYPE)                                                         \
    do {                                                                                                             \
        auto op = std::make_unique<JSOperation>(                                                                     \
                OPERATION,                                                                                           \
                OperationInputList{OperationInput(OP1, InputType::DOUBLE), OperationInput(OP2, InputType::DOUBLE)}); \
        auto c = std::make_unique<Constraint>();                                                                     \
        c->addRequiredCovariateId(OP1);                                                                              \
        c->addRequiredCovariateId(OP2);                                                                              \
        c->setType(TYPE);                                                                                            \
        c->setCheckOperation(std::move(op));                                                                         \
        DOMAIN->addConstraint(std::move(c));                                                                         \
    } while (0);

#define ADD_OP1_CONSTRAINT(DOMAIN, OPERATION, OP1, TYPE)                                                              \
    do {                                                                                                              \
        auto op =                                                                                                     \
                std::make_unique<JSOperation>(OPERATION, OperationInputList{OperationInput(OP1, InputType::DOUBLE)}); \
        auto c = std::make_unique<Constraint>();                                                                      \
        c->addRequiredCovariateId(OP1);                                                                               \
        c->setType(TYPE);                                                                                             \
        c->setCheckOperation(std::move(op));                                                                          \
        DOMAIN->addConstraint(std::move(c));                                                                          \
    } while (0);

enum class DataIntegrity
{
    //Use to create good covariate
    GoodDataGoodFormat,

    //Use to create bad Data with good format --> match partiallyCompatible
    BadDataGoodFormat,

    //Use to create bad data with bad format --> match comutationError
    BadDataBadFormat
};

enum class Result
{

    /// The patient covariates are incompatible with the drug model : hard constraints are not met
    Incompatible,

    /// The patient covariates are not totally compatible: soft constraints are not met
    PartiallyCompatible,

    /// The patient covariates are fully compatible with the drug model
    Compatible,

    /// A computation error occured
    ComputationError
};

static std::unique_ptr<DrugModelDomain> howManyConstraints(
        ConstraintType _firstConstraintType, ConstraintType _secondConstraintType, uint8_t _nbConstraint = 1)
{
    const int NB_CASE = 2;

    std::unique_ptr<DrugModelDomain> domain = std::make_unique<DrugModelDomain>();

    _nbConstraint = (_nbConstraint > NB_CASE) ? NB_CASE : _nbConstraint;

    for (int i = 0; i < _nbConstraint; i++) {
        switch (i) {

        case 0:
            ADD_OP1_CONSTRAINT(domain, "return Weight < 100;", "Weight", _firstConstraintType);
            break;

        case 1:
            ADD_OP1_CONSTRAINT(domain, "return Gist;", "Gist", _secondConstraintType);
            break;

        default:

            break;
        }
    }

    return std::unique_ptr<DrugModelDomain>(std::move(domain));
}

static DrugModel howManyCovariateDefinition(uint8_t _nbCovariateDefinition)
{
    const int NB_CASE = 4;

    DrugModel drugModel;

    CovariateDefinitions cDefinitions;

    _nbCovariateDefinition = (_nbCovariateDefinition > NB_CASE) ? NB_CASE : _nbCovariateDefinition;

    for (int i = 0; i < _nbCovariateDefinition; i++) {
        switch (i) {

        case 0:
            ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
            break;

        case 1:
            ADD_CDEF_NO_R(Gist, true, Standard, Bool, Direct, cDefinitions);
            break;

        case 2:
            ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);
            break;

        case 3:
            ADD_CDEF_NO_R_UNIT(Age, 3, AgeInDays, Int, Linear, "d", cDefinitions);
            break;

        default:

            break;
        }
    }


    for (auto& cDefinition : cDefinitions) {
        drugModel.addCovariate(std::move(cDefinition));
    }

    return drugModel;
}

static DrugTreatment howManyPatientCovariate(
        uint8_t _nbPatientCovariate, DataIntegrity _dataIntegrity = DataIntegrity::GoodDataGoodFormat)
{
    const int NB_CASE = 5;

    DrugTreatment drugTreatment;

    _nbPatientCovariate = (_nbPatientCovariate > NB_CASE) ? NB_CASE : _nbPatientCovariate;

    std::unique_ptr<PatientCovariate> covariate;

    std::string birthdate;
    std::string height;
    std::string weight;
    std::string sex;
    std::string gist;

    switch (_dataIntegrity) {

    case DataIntegrity::BadDataBadFormat:

        birthdate = "1000-05-08-10:00:00";
        height = "l";
        weight = "true";
        sex = "10h";
        gist = "truffe";

        break;

    case DataIntegrity::BadDataGoodFormat:
        //Consider as wrong due to actual constraints value
        birthdate = "1000-05-08T10:00:00";
        height = "250";
        weight = "200";
        sex = "0.5";
        gist = "false";
        break;

    case DataIntegrity::GoodDataGoodFormat:

        birthdate = "2020-05-08T10:00:00";
        height = "180";
        weight = "55";
        sex = "0";
        gist = "true";
        break;
    }



    for (int i = 0; i < _nbPatientCovariate; i++) {
        switch (i) {

        case 0:
            covariate = std::make_unique<PatientCovariate>(
                    "Weight", weight, DataType::Double, TucuUnit("kg"), DATE_TIME_NO_VAR(2020, 6, 8, 8, 0, 0));
            break;

        case 1:
            covariate = std::make_unique<PatientCovariate>(
                    "Gist", gist, DataType::Bool, TucuUnit("-"), DATE_TIME_NO_VAR(2020, 6, 9, 8, 0, 0));
            break;

        case 2:
            covariate = std::make_unique<PatientCovariate>(
                    "Height", height, DataType::Double, TucuUnit("cm"), DATE_TIME_NO_VAR(2020, 6, 10, 8, 0, 0));
            break;

        case 3:
            covariate = std::make_unique<PatientCovariate>(
                    "Birthdate", birthdate, DataType::Date, TucuUnit("-"), DATE_TIME_NO_VAR(2020, 6, 11, 8, 0, 0));
            break;

        case 4:
            covariate = std::make_unique<PatientCovariate>(
                    "Sex", sex, DataType::Bool, TucuUnit("-"), DATE_TIME_NO_VAR(2020, 6, 12, 8, 0, 0));
            break;

        default:

            break;
        }

        drugTreatment.addCovariate(std::move(covariate));
    }

    return drugTreatment;
}


static std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluateUniqueResult(
        DrugModel& _drugModel, DrugTreatment& _drugTreatment, Result result)
{
    DrugDomainConstraintsEvaluator evaluator;

    DrugDomainConstraintsEvaluator::Result rc;

    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

    rc = evaluator.evaluate(
            _drugModel,
            _drugTreatment,
            DATE_TIME_NO_VAR(2020, 5, 7, 8, 0, 0),
            DATE_TIME_NO_VAR(2020, 6, 12, 8, 0, 0),
            evaluationResults);

    switch (result) {

    case Result::Compatible:
        EXPECT_TRUE(rc == DrugDomainConstraintsEvaluator::Result::Compatible);
        break;

    case Result::Incompatible:
        EXPECT_TRUE(rc == DrugDomainConstraintsEvaluator::Result::Incompatible);
        break;

    case Result::ComputationError:
        EXPECT_TRUE(rc == DrugDomainConstraintsEvaluator::Result::ComputationError);
        break;

    case Result::PartiallyCompatible:
        EXPECT_TRUE(rc == DrugDomainConstraintsEvaluator::Result::PartiallyCompatible);
        break;
    }

    return evaluationResults;
}

static void compatibleTests(ConstraintType _firstConstraintType, ConstraintType _secondConstraintType)
{
    const Result RESULT_TYPE = Result::Compatible;
    const DrugDomainConstraintsEvaluator::Result CONSTRAINTS_RESULT_TYPE =
            DrugDomainConstraintsEvaluator::Result::Compatible;

    uint8_t nbConstraints = 2;
    uint8_t nbCovariateDefinition = 4;
    uint8_t nbPatientCovariate = 4;

    std::unique_ptr<DrugModelDomain> domain;
    DrugModel drugModel;
    DrugTreatment drugTreatment;

    domain = howManyConstraints(_firstConstraintType, _secondConstraintType, nbConstraints);

    drugModel = howManyCovariateDefinition(nbCovariateDefinition);

    drugModel.setDomain(std::move(domain));


    //Generate wrong covariate
    drugTreatment = howManyPatientCovariate(nbPatientCovariate, DataIntegrity::GoodDataGoodFormat);


    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults =
            evaluateUniqueResult(drugModel, drugTreatment, RESULT_TYPE);


    for (const auto evalResult : evaluationResults) {
        EXPECT_TRUE(evalResult.m_result == CONSTRAINTS_RESULT_TYPE);
    }
}

static void computationErrorTests(ConstraintType _firstConstraintType, ConstraintType _secondConstraintType)
{
    const Result RESULT_TYPE = Result::ComputationError;
    const DrugDomainConstraintsEvaluator::Result CONSTRAINTS_RESULT_TYPE =
            DrugDomainConstraintsEvaluator::Result::ComputationError;

    uint8_t nbConstraints = 2;
    uint8_t nbCovariateDefinition = 2;
    uint8_t nbPatientCovariate = 2;

    std::unique_ptr<DrugModelDomain> domain;
    DrugModel drugModel;
    DrugTreatment drugTreatment;

    domain = howManyConstraints(_firstConstraintType, _secondConstraintType, nbConstraints);

    drugModel = howManyCovariateDefinition(nbCovariateDefinition);

    drugModel.setDomain(std::move(domain));


    drugTreatment = howManyPatientCovariate(nbPatientCovariate, DataIntegrity::BadDataBadFormat);


    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults =
            evaluateUniqueResult(drugModel, drugTreatment, RESULT_TYPE);


    for (const auto evalResult : evaluationResults) {
        EXPECT_EQ(evalResult.m_result, CONSTRAINTS_RESULT_TYPE);
    }
}

static void incompatibleTests(ConstraintType _firstConstraintType, ConstraintType _secondConstraintType)
{

    const Result RESULT_TYPE = Result::Incompatible;
    const DrugDomainConstraintsEvaluator::Result CONSTRAINTS_RESULT_TYPE =
            DrugDomainConstraintsEvaluator::Result::Incompatible;

    uint8_t nbConstraints = 2;
    uint8_t nbCovariateDefinition = 2;
    uint8_t nbPatientCovariate = 2;

    std::unique_ptr<DrugModelDomain> domain;
    DrugModel drugModel;
    DrugTreatment drugTreatment;

    domain = howManyConstraints(_firstConstraintType, _secondConstraintType, nbConstraints);

    drugModel = howManyCovariateDefinition(nbCovariateDefinition);

    drugModel.setDomain(std::move(domain));


    //Generate wrong covariate
    drugTreatment = howManyPatientCovariate(nbPatientCovariate, DataIntegrity::BadDataGoodFormat);


    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults =
            evaluateUniqueResult(drugModel, drugTreatment, RESULT_TYPE);

    //Must define Specific test for results vector evaluationResults


    for (const auto evalResult : evaluationResults) {
        if (evalResult.m_result != CONSTRAINTS_RESULT_TYPE) {
            std::cout << " coucou";
        }
        EXPECT_EQ(evalResult.m_result, CONSTRAINTS_RESULT_TYPE);
    }
}

static void partiallyCompatibleTests()
{
    const Result RESULT_TYPE = Result::PartiallyCompatible;
    const DrugDomainConstraintsEvaluator::Result CONSTRAINTS_RESULT_TYPE =
            DrugDomainConstraintsEvaluator::Result::PartiallyCompatible;

    ConstraintType _firstConstraint = ConstraintType::SOFT;
    ConstraintType _secondConstraint = ConstraintType::SOFT;

    uint8_t nbConstraints = 2;
    uint8_t nbCovariateDefinition = 2;
    uint8_t nbPatientCovariate = 2;

    std::unique_ptr<DrugModelDomain> domain;
    DrugModel drugModel;
    DrugTreatment drugTreatment;

    domain = howManyConstraints(_firstConstraint, _secondConstraint, nbConstraints);

    drugModel = howManyCovariateDefinition(nbCovariateDefinition);

    drugModel.setDomain(std::move(domain));


    //Generate wrong covariate
    drugTreatment = howManyPatientCovariate(nbPatientCovariate, DataIntegrity::BadDataGoodFormat);


    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults =
            evaluateUniqueResult(drugModel, drugTreatment, RESULT_TYPE);

    //Must define Specific test for results vector evaluationResults


    for (const auto evalResult : evaluationResults) {
        EXPECT_TRUE(evalResult.m_result == CONSTRAINTS_RESULT_TYPE);
    }
}

static void imatinibTest()
{

    std::unique_ptr<DrugModelDomain> domain = std::make_unique<DrugModelDomain>();
    // Computed parameters
    ADD_OP1_CONSTRAINT(
            domain, "return ((bodyweight >= 44) && (bodyweight <= 110));", "bodyweight", ConstraintType::SOFT);
    ADD_OP1_CONSTRAINT(domain, "return ((age <= 88) && (age >= 20));", "age", ConstraintType::HARD);

    DrugModel drugModel;

    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_W_R_UNIT(bodyweight, 70, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_NO_R_UNIT(sex, 0.5, Sex, Int, Linear, "y", cDefinitions);
    ADD_CDEF_NO_R_UNIT(age, 50, AgeInYears, Int, Linear, "y", cDefinitions);

    drugModel.addCovariate(std::move(cDefinitions[0]));
    drugModel.addCovariate(std::move(cDefinitions[1]));
    drugModel.addCovariate(std::move(cDefinitions[2]));
    drugModel.addCovariate(std::move(cDefinitions[3]));

    drugModel.setDomain(std::move(domain));


    DrugTreatment drugTreatment;

    drugTreatment.addCovariate(std::make_unique<PatientCovariate>(
            "Birthdate", "1990-01-01T00:00:00", DataType::Date, TucuUnit("-"), DATE_TIME_NO_VAR(2020, 5, 8, 8, 0, 0)));
    drugTreatment.addCovariate(std::make_unique<PatientCovariate>(
            "bodyweight", "60", DataType::Double, TucuUnit("kg"), DATE_TIME_NO_VAR(2020, 5, 9, 8, 0, 0)));



    DrugDomainConstraintsEvaluator evaluator;

    DrugDomainConstraintsEvaluator::Result rc;

    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

    rc = evaluator.evaluate(
            drugModel,
            drugTreatment,
            DATE_TIME_NO_VAR(2020, 5, 7, 8, 0, 0),
            DATE_TIME_NO_VAR(2020, 6, 12, 8, 0, 0),
            evaluationResults);

    EXPECT_TRUE(rc == DrugDomainConstraintsEvaluator::Result::Compatible);
}

TEST(Core_TestDrugDomainConstraintsEvaluator, SimpleTest1)
{
    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Age, 50, AgeInYears, Int, Linear, "y", cDefinitions);


    // Set of covariate events.
    CovariateSeries cSeries;
    cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), varToValue(true)));
    cSeries.push_back(CovariateEvent(*(cDefinitions[1]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 15));
    cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 0, 0), 111));
    cSeries.push_back(CovariateEvent(*(cDefinitions[3]), DATE_TIME_NO_VAR(2017, 8, 15, 8, 10, 0), 51));
    cSeries.push_back(CovariateEvent(*(cDefinitions[0]), DATE_TIME_NO_VAR(2017, 8, 16, 8, 0, 0), varToValue(false)));
    cSeries.push_back(CovariateEvent(*(cDefinitions[2]), DATE_TIME_NO_VAR(2017, 8, 16, 9, 0, 0), 123));


    DrugModelDomain domain;

    // Computed parameters
    ADD_OP2_CONSTRAINT(domain, "return Gist;", "Gist", "Weight", ConstraintType::HARD);
    ADD_OP2_CONSTRAINT(domain, "return Weight < 100;", "Weight", "Gist", ConstraintType::HARD);



    DrugDomainConstraintsEvaluator evaluator;

    DrugDomainConstraintsEvaluator::Result rc;

    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

    rc = evaluator.evaluate(
            domain,
            cSeries,
            DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
            DATE_TIME_NO_VAR(2018, 8, 17, 8, 0, 0),
            evaluationResults);


    EXPECT_TRUE(rc == DrugDomainConstraintsEvaluator::Result::Incompatible);
}

TEST(Core_TestDrugDomainConstraintsEvaluator, SimpleTest2)
{
    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_NO_R(Gist, true, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Age, 3, AgeInWeeks, Int, Linear, "w", cDefinitions);


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

    rc = evaluator.evaluate(
            domain,
            cSeries,
            DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
            DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
            evaluationResults);

    for (const auto result : evaluationResults) {
        ASSERT_EQ(result.m_result, DrugDomainConstraintsEvaluator::Result::Compatible);
    }

    ASSERT_EQ(rc, DrugDomainConstraintsEvaluator::Result::Compatible);
}

TEST(Core_TestDrugDomainConstraintsEvaluator, SimpleTest3)
{
    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_NO_R(Gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_W_R_UNIT(Weight, 3.5, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Height, 100, Standard, Double, Linear, "cm", cDefinitions);
    ADD_CDEF_NO_R_UNIT(Age, 3, AgeInDays, Int, Linear, "w", cDefinitions);

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

    rc = evaluator.evaluate(
            domain,
            cSeries,
            DATE_TIME_NO_VAR(2017, 8, 14, 8, 0, 0),
            DATE_TIME_NO_VAR(2017, 8, 17, 8, 0, 0),
            evaluationResults);

    ASSERT_EQ(evaluationResults.size(), static_cast<size_t>(2));
    ASSERT_EQ(evaluationResults[0].m_result, DrugDomainConstraintsEvaluator::Result::PartiallyCompatible);
    ASSERT_EQ(evaluationResults[1].m_result, DrugDomainConstraintsEvaluator::Result::Compatible);

    ASSERT_EQ(rc, DrugDomainConstraintsEvaluator::Result::PartiallyCompatible);
}

TEST(Core_TestDrugDomainConstraintsEvaluator, SimpleTest4)
{
    imatinibTest();


    compatibleTests(ConstraintType::SOFT, ConstraintType::SOFT);
    compatibleTests(ConstraintType::SOFT, ConstraintType::HARD);
    compatibleTests(ConstraintType::HARD, ConstraintType::SOFT);
    compatibleTests(ConstraintType::HARD, ConstraintType::HARD);



    //        computationErrorTests(ConstraintType::SOFT, ConstraintType::SOFT);
    //        computationErrorTests(ConstraintType::HARD, ConstraintType::SOFT);
    //        computationErrorTests(ConstraintType::SOFT, ConstraintType::HARD);
    //        computationErrorTests(ConstraintType::HARD, ConstraintType::HARD);



    incompatibleTests(ConstraintType::HARD, ConstraintType::HARD);
    //incompatibleTests(ConstraintType::HARD, ConstraintType::SOFT);

    // To obtain partiallyCompatible, only soft constraint without any change between them
    partiallyCompatibleTests();
}

TEST(Core_TestDrugDomainConstraintsEvaluator, SimpleTest5)
{
    std::unique_ptr<DrugModelDomain> domain = std::make_unique<DrugModelDomain>();
    // Computed parameters
    ADD_OP1_CONSTRAINT(
            domain, "return ((bodyweight >= 44) && (bodyweight <= 110));", "bodyweight", ConstraintType::SOFT);
    ADD_OP1_CONSTRAINT(domain, "return ((age <= 88) && (age >= 20));", "age", ConstraintType::MANDATORYHARD);
    ADD_OP1_CONSTRAINT(domain, "return gist;", "gist", ConstraintType::HARD);

    DrugModel drugModel;

    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_W_R_UNIT(bodyweight, 70, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R(gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_NO_R_UNIT(age, 50, AgeInYears, Int, Linear, "y", cDefinitions);

    drugModel.addCovariate(std::move(cDefinitions[0]));
    drugModel.addCovariate(std::move(cDefinitions[1]));
    drugModel.addCovariate(std::move(cDefinitions[2]));

    drugModel.setDomain(std::move(domain));


    DrugTreatment drugTreatment;

    drugTreatment.addCovariate(std::make_unique<PatientCovariate>(
            "bodyweight",
            "60",
            DataType::Double,
            TucuUnit("kg"),
            DATE_TIME_NO_VAR(2020, 5, 9, 8, 0, 0))); // DM constraint : SOFT
    drugTreatment.addCovariate(std::make_unique<PatientCovariate>(
            "gist",
            "true",
            DataType::Bool,
            TucuUnit("-"),
            DATE_TIME_NO_VAR(2020, 5, 10, 8, 0, 0))); // DM constraint : HARD

    DrugDomainConstraintsEvaluator evaluator;

    DrugDomainConstraintsEvaluator::Result rc;

    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

    rc = evaluator.evaluate(
            drugModel,
            drugTreatment,
            DATE_TIME_NO_VAR(2020, 5, 7, 8, 0, 0),
            DATE_TIME_NO_VAR(2020, 6, 12, 10, 0, 0),
            evaluationResults);

    // Covariate in drugtreatment missing knowing that its constraint is hard
    ASSERT_EQ(rc, DrugDomainConstraintsEvaluator::Result::Incompatible);
}

TEST(Core_TestDrugDomainConstraintsEvaluator, SimpleTest6)
{
    std::unique_ptr<DrugModelDomain> domain = std::make_unique<DrugModelDomain>();
    // Computed parameters
    ADD_OP1_CONSTRAINT(
            domain, "return ((bodyweight >= 44) && (bodyweight <= 110));", "bodyweight", ConstraintType::HARD);
    ADD_OP1_CONSTRAINT(domain, "return ((age <= 88) && (age >= 20));", "age", ConstraintType::SOFT);
    ADD_OP1_CONSTRAINT(domain, "return gist;", "gist", ConstraintType::HARD);

    DrugModel drugModel;

    CovariateDefinitions cDefinitions;
    // Covariates of interest.
    ADD_CDEF_W_R_UNIT(bodyweight, 70, Standard, Double, Linear, Tucuxi::Common::days(1), "kg", cDefinitions);
    ADD_CDEF_NO_R(gist, false, Standard, Bool, Direct, cDefinitions);
    ADD_CDEF_NO_R_UNIT(age, 50, AgeInYears, Int, Linear, "y", cDefinitions);

    drugModel.addCovariate(std::move(cDefinitions[0]));
    drugModel.addCovariate(std::move(cDefinitions[1]));
    drugModel.addCovariate(std::move(cDefinitions[2]));

    drugModel.setDomain(std::move(domain));


    DrugTreatment drugTreatment;

    drugTreatment.addCovariate(std::make_unique<PatientCovariate>(
            "bodyweight",
            "60",
            DataType::Double,
            TucuUnit("kg"),
            DATE_TIME_NO_VAR(2020, 5, 9, 8, 0, 0))); // DM constraint : HARD
    drugTreatment.addCovariate(std::make_unique<PatientCovariate>(
            "gist",
            "true",
            DataType::Bool,
            TucuUnit("-"),
            DATE_TIME_NO_VAR(2020, 5, 10, 8, 0, 0))); // DM constraint : HARD

    DrugDomainConstraintsEvaluator evaluator;

    DrugDomainConstraintsEvaluator::Result rc;

    std::vector<DrugDomainConstraintsEvaluator::EvaluationResult> evaluationResults;

    rc = evaluator.evaluate(
            drugModel,
            drugTreatment,
            DATE_TIME_NO_VAR(2020, 5, 7, 8, 0, 0),
            DATE_TIME_NO_VAR(2020, 6, 12, 10, 0, 0),
            evaluationResults);

    // Covariate in drugtreatment missing knowing that its constraint is soft
    ASSERT_EQ(rc, DrugDomainConstraintsEvaluator::Result::Compatible);
}
