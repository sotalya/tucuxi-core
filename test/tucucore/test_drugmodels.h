/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_DRUGMODELS_H
#define TEST_DRUGMODELS_H


#include "fructose/fructose.h"

#include "tucucore/drugmodel.h"

using namespace Tucuxi::Core;



struct TestDrugModels : public fructose::test_base<TestDrugModels>
{
    TestDrugModels() { }

    DrugModel *buildImatinib()
    {
        DrugModel *model;
        model = new DrugModel();


        // Imatinib parameters, as in the XML drug file
        Tucuxi::Core::ParameterDefinitions parameterDefs;
        Operation *opV = new JSOperation(" \
                                         theta2=V; \
                theta8=46.2; \
        tvv = theta2+theta8*sex-theta8*(1-sex); \
        return tvv;",
        { OperationInput("V", InputType::DOUBLE),
                    OperationInput("sex", InputType::DOUBLE)});

        parameterDefs.push_back(std::unique_ptr<ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V", 347, opV, Tucuxi::Core::ParameterDefinition::ErrorModel::None)));
        model->addDispositionParameter(new Tucuxi::Core::ParameterDefinition("V", 347, opV, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
        Operation *opCl = new JSOperation(" \
                                          theta1=CL; \
                theta4=5.42; \
        theta5=1.49; \
        theta6=-5.81; \
        theta7=-.806; \
        \
        MEANBW=70; \
        FBW=(weight-MEANBW)/MEANBW; \
        \
        MEANAG=50; \
        FAGE=(age-MEANAG)/MEANAG; \
        \
        if (gist) \
            PATH=1; \
        else \
            PATH=0; \
        \
        PATH = gist; \
        \
        MALE = sex; \
        \
        TVCL = theta1+theta4*FBW+theta5*MALE-theta5*(1-MALE)+theta6*FAGE + theta7*PATH-theta7*(1-PATH); \
        return TVCL; \
        ",
        { OperationInput("CL", InputType::DOUBLE),


                    OperationInput("sex", InputType::DOUBLE),
                    OperationInput("weight", InputType::DOUBLE),
                    OperationInput("age", InputType::DOUBLE),
                    OperationInput("gist", InputType::BOOL)});

        parameterDefs.push_back(std::unique_ptr<ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("CL", 15.106, opCl, Tucuxi::Core::ParameterDefinition::ErrorModel::None)));
        model->addDispositionParameter(new Tucuxi::Core::ParameterDefinition("CL", 15.106, opCl, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
        //model->setDispositionParameters(parameterDefs);


        ActiveSubstance *activeSubstance;
        activeSubstance = new ActiveSubstance();
        activeSubstance->setPkModelId("linear.1comp.macro");

        Analyte analyte;
        analyte.setName("Imatinib");
        analyte.setAnalyteId("imatinib");

        activeSubstance->addAnalyte(analyte);


        {
            FormulationAndRoute *formulationAndRoute;
            formulationAndRoute = new FormulationAndRoute();
            formulationAndRoute->addAbsorptionParameter(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            formulationAndRoute->addAbsorptionParameter(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterDefinition::ErrorModel::None));
            formulationAndRoute->setRoute(Route::Oral);
            formulationAndRoute->setFormulation("To be defined");
            formulationAndRoute->setAbsorptionModel(AbsorptionModel::EXTRAVASCULAR);

            formulationAndRoute->addActiveSubstance(activeSubstance);

            model->addFormulationAndRoute(formulationAndRoute);
        }

        {
            FormulationAndRoute *formulationAndRoute;
            formulationAndRoute = new FormulationAndRoute();
            formulationAndRoute->setRoute(Route::IntravenousBolus);
            formulationAndRoute->setFormulation("To be defined");
            formulationAndRoute->setAbsorptionModel(AbsorptionModel::INTRAVASCULAR);

            formulationAndRoute->addActiveSubstance(activeSubstance);

            model->addFormulationAndRoute(formulationAndRoute);
        }

        {
            FormulationAndRoute *formulationAndRoute;
            formulationAndRoute = new FormulationAndRoute();
            formulationAndRoute->setRoute(Route::IntravenousDrip);
            formulationAndRoute->setFormulation("To be defined");
            formulationAndRoute->setAbsorptionModel(AbsorptionModel::INFUSION);

            formulationAndRoute->addActiveSubstance(activeSubstance);

            model->addFormulationAndRoute(formulationAndRoute);
        }

        model->addCovariate(new Tucuxi::Core::CovariateDefinition("weight", 70, nullptr, CovariateType::Standard));
        model->addCovariate(new Tucuxi::Core::CovariateDefinition("gist", 0, nullptr, CovariateType::Standard));
        model->addCovariate(new Tucuxi::Core::CovariateDefinition("sex", 0.5, nullptr, CovariateType::Standard));
        model->addCovariate(new Tucuxi::Core::CovariateDefinition("age", 50, nullptr, CovariateType::Standard));


        SigmaResidualErrorModel *errorModel;
        errorModel = new SigmaResidualErrorModel();
        Sigma sigma(1);
        sigma(0) = 0.3138;
        errorModel->setErrorModel(SigmaResidualErrorModel::ResidualErrorType::PROPORTIONAL);
        errorModel->setSigma(sigma);

        model->setResidualErrorMoedl(errorModel);


        // Add targets

        SubTargetDefinition cMin("cMin", 750.0, nullptr);
        SubTargetDefinition cMax("cMax", 1500.0, nullptr);
        SubTargetDefinition cBest("cBest", 1000.0, nullptr);
        TargetDefinition *target = new TargetDefinition(TargetType::Residual,
                                                        &cMin,
                                                        &cMax,
                                                        &cBest,
                                                        new SubTargetDefinition("cBest", 1000.0, nullptr),
                                                        new SubTargetDefinition("cBest", 1000.0, nullptr),
                                                        new SubTargetDefinition("cBest", 1000.0, nullptr));

        model->addTarget(target);


        // Add possible dosages

        return model;
    }

    void testImatinib(const std::string& /* _testName */)
    {
        DrugModel* imatinib;
        imatinib = buildImatinib();

        fructose_assert(imatinib != nullptr);
    }

};

#endif // TEST_DRUGMODELS_H
