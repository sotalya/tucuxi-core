/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TEST_DRUGMODELS_H
#define TEST_DRUGMODELS_H


#include "fructose/fructose.h"

#include "tucucore/drugmodel/drugmodel.h"

using namespace Tucuxi::Core;



struct TestDrugModels : public fructose::test_base<TestDrugModels>
{
    TestDrugModels() { }

    DrugModel *buildImatinib()
    {
        DrugModel *model;
        model = new DrugModel();




        // The following constraint is for tests only. Needs to be modified according to the paper
        Operation *constraint = new JSOperation(" \
                                         return (age > 0);",
        { OperationInput("age", InputType::DOUBLE)});
        std::unique_ptr<DrugModelDomain> drugDomain(new DrugModelDomain(std::unique_ptr<Operation>(constraint)));

        model->setDomain(drugDomain);


        model->addCovariate(
                    std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                        new Tucuxi::Core::CovariateDefinition("weight", 70, nullptr, CovariateType::Standard)));
        model->addCovariate(
                    std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                        new Tucuxi::Core::CovariateDefinition("gist", 0, nullptr, CovariateType::Standard)));
        model->addCovariate(
                    std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                        new Tucuxi::Core::CovariateDefinition("sex", 0.5, nullptr, CovariateType::Standard)));
        model->addCovariate(
                    std::unique_ptr<Tucuxi::Core::CovariateDefinition>(
                        new Tucuxi::Core::CovariateDefinition("age", 50, nullptr, CovariateType::Standard)));

        std::unique_ptr<AnalyteSet> analyteSet(new AnalyteSet());

        analyteSet->setPkModelId("linear.1comp.macro");


        ActiveSubstance *analyte = new ActiveSubstance();
        analyte->setAnalyteId("imatinib");

        SigmaResidualErrorModel* errorModel = new SigmaResidualErrorModel();
//        std::unique_ptr<SigmaResidualErrorModel> errorModel(new SigmaResidualErrorModel());
        Sigma sigma(1);
        sigma(0) = 0.3138;
        errorModel->setErrorModel(SigmaResidualErrorModel::ResidualErrorType::PROPORTIONAL);
        errorModel->setSigma(sigma);

        std::unique_ptr<IResidualErrorModel> err(errorModel);

        analyte->setResidualErrorModel(err);


        // Add targets

        std::unique_ptr<SubTargetDefinition> cMin(new SubTargetDefinition("cMin", 750.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cMax(new SubTargetDefinition("cMax", 1500.0, nullptr));
        std::unique_ptr<SubTargetDefinition> cBest(new SubTargetDefinition("cBest", 1000.0, nullptr));
        TargetDefinition *target = new TargetDefinition(TargetType::Residual,
                                                        std::move(cMin),
                                                        std::move(cMax),
                                                        std::move(cBest),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("cBest", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("cBest", 1000.0, nullptr)),
                                                        std::unique_ptr<SubTargetDefinition>(new SubTargetDefinition("cBest", 1000.0, nullptr)));

        std::unique_ptr<TargetDefinition> targetPtr(target);
        analyte->addTarget(targetPtr);



        analyteSet->addAnalyte(std::unique_ptr<Analyte>(analyte));



        std::unique_ptr<ParameterSetDefinition> dispositionParameters(new ParameterSetDefinition());

        // Imatinib parameters, as in the XML drug file
        Operation *opV = new JSOperation(" \
                                         theta2=V; \
                theta8=46.2; \
        tvv = theta2+theta8*sex-theta8*(1-sex); \
        return tvv;",
        { OperationInput("V", InputType::DOUBLE),
                    OperationInput("sex", InputType::DOUBLE)});

        std::unique_ptr<ParameterDefinition> PV(new Tucuxi::Core::ParameterDefinition("V", 347, opV, Tucuxi::Core::ParameterVariabilityType::None));
        dispositionParameters->addParameter(PV);
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

        std::unique_ptr<ParameterDefinition> PCL(new Tucuxi::Core::ParameterDefinition("CL", 15.106, opCl, Tucuxi::Core::ParameterVariabilityType::None));
        dispositionParameters->addParameter(PCL);

        dispositionParameters->addCorrelation(Correlation("CL", "V", 0.798));
        analyteSet->setDispositionParameters(std::move(dispositionParameters));


        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::EXTRAVASCULAR);


            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());
            std::unique_ptr<ParameterDefinition> PKa(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(PKa);
            std::unique_ptr<ParameterDefinition> PF(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
            absorptionParameters->addParameter(PF);

            association->setAbsorptionParameters(std::move(absorptionParameters));
            std::unique_ptr<FormulationAndRoute> formulationAndRoute(new FormulationAndRoute());
            formulationAndRoute->setRoute(Route::Oral);
            formulationAndRoute->setFormulation("To be defined");
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));

            SpecificDoses* validDoses = new SpecificDoses(Unit("mg"), MultiAnalyteDose(400));
            validDoses->addDose(MultiAnalyteDose(100));
            validDoses->addDose(MultiAnalyteDose(200));
            validDoses->addDose(MultiAnalyteDose(300));
            validDoses->addDose(MultiAnalyteDose(400));
            validDoses->addDose(MultiAnalyteDose(600));
            validDoses->addDose(MultiAnalyteDose(800));

            std::unique_ptr<ValidDoses> doses(validDoses);

            formulationAndRoute->setValidDoses(std::move(doses));

            SpecificDurations* validIntervals = new SpecificDurations(24h);
            validIntervals->addDuration(Duration(12h));
            validIntervals->addDuration(Duration(24h));

            std::unique_ptr<ValidDurations> intervals(validIntervals);
            formulationAndRoute->setValidIntervals(std::move(intervals));


            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::INTRAVASCULAR);


            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());

            association->setAbsorptionParameters(std::move(absorptionParameters));
            std::unique_ptr<FormulationAndRoute> formulationAndRoute(new FormulationAndRoute());
            formulationAndRoute->setRoute(Route::IntravenousBolus);
            formulationAndRoute->setFormulation("To be defined");
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));

            // These are the extravascular doses and intervals. Only added the infusion times
            std::unique_ptr<SpecificDoses> validDoses(new SpecificDoses(Unit("mg"), MultiAnalyteDose(400)));
            validDoses->addDose(MultiAnalyteDose(100));
            validDoses->addDose(MultiAnalyteDose(200));
            validDoses->addDose(MultiAnalyteDose(300));
            validDoses->addDose(MultiAnalyteDose(400));
            validDoses->addDose(MultiAnalyteDose(600));
            validDoses->addDose(MultiAnalyteDose(800));

            formulationAndRoute->setValidDoses(std::move(validDoses));

            std::unique_ptr<SpecificDurations> validIntervals(new SpecificDurations(24h));
            validIntervals->addDuration(Duration(12h));
            validIntervals->addDuration(Duration(24h));
            formulationAndRoute->setValidIntervals(std::move(validIntervals));

            // This is just here as an example.
            std::unique_ptr<SpecificDurations> validInfusionTimes(new SpecificDurations(1h));
            validInfusionTimes->addDuration(Duration(30min));
            validInfusionTimes->addDuration(Duration(1h));
            formulationAndRoute->setValidInfusionTimes(std::move(validInfusionTimes));

            model->addFormulationAndRoute(std::move(formulationAndRoute));

        }

        {
            AnalyteSetToAbsorptionAssociation *association;
            association = new AnalyteSetToAbsorptionAssociation(*analyteSet);
            association->setAbsorptionModel(AbsorptionModel::INFUSION);


            std::unique_ptr<ParameterSetDefinition> absorptionParameters(new ParameterSetDefinition());

            association->setAbsorptionParameters(std::move(absorptionParameters));
            std::unique_ptr<FormulationAndRoute> formulationAndRoute(new FormulationAndRoute());
            formulationAndRoute->setRoute(Route::IntravenousDrip);
            formulationAndRoute->setFormulation("To be defined");
            formulationAndRoute->addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation>(association));

            // These are the extravascular doses and intervals
            std::unique_ptr<SpecificDoses> validDoses(new SpecificDoses(Unit("mg"), MultiAnalyteDose(400)));
            validDoses->addDose(MultiAnalyteDose(100));
            validDoses->addDose(MultiAnalyteDose(200));
            validDoses->addDose(MultiAnalyteDose(300));
            validDoses->addDose(MultiAnalyteDose(400));
            validDoses->addDose(MultiAnalyteDose(600));
            validDoses->addDose(MultiAnalyteDose(800));

            formulationAndRoute->setValidDoses(std::move(validDoses));

            std::unique_ptr<SpecificDurations> validIntervals(new SpecificDurations(24h));
            validIntervals->addDuration(Duration(12h));
            validIntervals->addDuration(Duration(24h));
            formulationAndRoute->setValidIntervals(std::move(validIntervals));

            model->addFormulationAndRoute(std::move(formulationAndRoute));
        }

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
