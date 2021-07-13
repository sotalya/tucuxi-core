#ifndef TEST_PARAMETER_H
#define TEST_PARAMETER_H


#include "fructose/fructose.h"

#include "tucucore/pkmodel.h"

#include "tucucore/pkmodels/onecompartmentbolus.h"
#include "tucucore/pkmodels/onecompartmentinfusion.h"
#include "tucucore/pkmodels/onecompartmentextra.h"
#include "tucucore/pkmodels/twocompartmentbolus.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/pkmodels/twocompartmentextra.h"
#include "tucucore/pkmodels/threecompartmentbolus.h"
#include "tucucore/pkmodels/threecompartmentinfusion.h"
#include "tucucore/pkmodels/threecompartmentextra.h"

using namespace Tucuxi::Core;

struct TestParameter : public fructose::test_base<TestParameter>
{
    TestParameter() { }

    /// \brief Test the function Parameter::applyEta().
    void testApplyEta(const std::string& /* _testName */)
    {
        {
            // Test Proportional variability
            ParameterDefinition pDef("pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Proportional, 1.0));
            Parameter p(pDef, 5.0);
            bool valid = p.applyEta(2.0);
            fructose_assert_eq(valid, true);
            fructose_assert_eq(p.getValue(), 15.0);
        }
        {
            // Test Exponential variability
            ParameterDefinition pDef("pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 1.0));
            Parameter p(pDef, 5.0);
            bool valid = p.applyEta(2.0);
            fructose_assert_eq(valid, true);
            fructose_assert_eq(p.getValue(), 5.0 * std::exp(2.0));
        }
        {
            // Test LogNormal variability
            ParameterDefinition pDef("pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::LogNormal, 1.0));
            Parameter p(pDef, 5.0);
            bool valid = p.applyEta(2.0);
            fructose_assert_eq(valid, true);
            fructose_assert_eq(p.getValue(), 5.0 * std::exp(2.0));
        }
        {
            // Test Normal variability
            ParameterDefinition pDef("pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Normal, 1.0));
            Parameter p(pDef, 5.0);
            bool valid = p.applyEta(2.0);
            fructose_assert_eq(valid, true);
            fructose_assert_eq(p.getValue(), 7.0);
        }
        {
            // Test logit variability
            ParameterDefinition pDef("pid", 0.5, std::make_unique<ParameterVariability>(ParameterVariabilityType::Logit, 1.0));
            Parameter p(pDef, 0.6);
            bool valid = p.applyEta(2.0);
            fructose_assert_eq(valid, true);
            fructose_assert_eq(p.getValue(), 1.0 / (1.0 + std::exp(-(std::log(0.6/(1.0 - 0.6)) + 2.0))));
        }
        {
            // Test logit variability with wrong parameter value
            ParameterDefinition pDef("pid", 0.5, std::make_unique<ParameterVariability>(ParameterVariabilityType::Logit, 1.0));
            Parameter p(pDef, 1.6);
            bool valid = p.applyEta(2.0);
            fructose_assert_eq(valid, false);
        }
    }

};

#endif // TEST_PARAMETER_H
