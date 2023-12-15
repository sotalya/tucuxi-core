/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_CORE_GENERALEXTRACTOR_H
#define TUCUXI_CORE_GENERALEXTRACTOR_H

#include "tucucommon/datetime.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/covariateevent.h"
#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/sample.h"

namespace Tucuxi {
namespace Core {

class ComputingRequest;
class DrugModel;
class ComputingTrait;
class ParameterSeries;
class ComputingTraitStandard;
class PkModel;
class HalfLife;
class PkModelCollection;
class DrugTreatment;

class GeneralExtractor
{
public:
    GeneralExtractor();

    Duration secureStartDuration(const HalfLife& _halfLife);

    ComputingStatus checkSamplesValidity(const DosageHistory& _dosageHistory, const Samples& _samples);

    ComputingStatus extractAposterioriEtas(
            Etas& _etas,
            const ComputingRequest& _request,
            PredictionParameterType _parameterType,
            const AnalyteGroupId& _analyteGroupId,
            const IntakeSeries& _intakeSeries,
            const ParameterSetSeries& _parameterSeries,
            const Tucuxi::Core::CovariateSeries& _covariateSeries,
            const Common::DateTime& _calculationStartTime,
            const Common::DateTime& _endTime,
            Value& _negativeLogLikelihood);

    ComputingStatus extractOmega(
            const DrugModel& _drugModel,
            const AnalyteGroupId& _analyteGroupId,
            std::vector<const FullFormulationAndRoute*>& _formulationAndRoutes,
            OmegaMatrix& _omega);

    ComputingStatus generalExtractions(
            const Tucuxi::Core::ComputingTraitStandard* _traits,
            const DrugModel& _drugModel,
            const DosageHistory& _dosageHistory,
            const Samples& _samples,
            const PatientVariates& _patientVariates,
            const PkModelCollection* _modelCollection,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            GroupsIntakeSeries& _intakeSeries,
            CovariateSeries& _covariatesSeries,
            GroupsParameterSetSeries& _parameterSeries,
            Common::DateTime& _calculationStartTime,
            const Common::DateTime& _covariateEndTime = Common::DateTime::undefinedDateTime());

    ComputingStatus generalExtractions(
            const Tucuxi::Core::ComputingTraitStandard* _traits,
            const ComputingRequest& _request,
            const PkModelCollection* _modelCollection,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel,
            GroupsIntakeSeries& _intakeSeries,
            CovariateSeries& _covariatesSeries,
            GroupsParameterSetSeries& _parameterSeries,
            Common::DateTime& _calculationStartTime,
            const Common::DateTime& _covariateEndTime = Common::DateTime::undefinedDateTime());

    std::vector<const FullFormulationAndRoute*> extractFormulationAndRoutes(
            const DrugModel& _drugModel, const IntakeSeries& _intakeSeries);

    ComputingStatus convertAnalytes(
            IntakeSeries& _intakeSeries, const DrugModel& _drugModel, const AnalyteSet* _analyteGroup);

    ComputingStatus extractParameters(
            const std::vector<FormulationAndRoute>& _formulationsAndRoutes,
            const AnalyteSets& _analyteSets,
            const DrugModel& _drugModel,
            const CovariateSeries& _covariatesSeries,
            const DateTime& _start,
            const DateTime& _end,
            PredictionParameterType _parametersType,
            GroupsParameterSetSeries& _parameterSeries);


    ComputingStatus extractPkModel(
            const DrugModel& _drugModel,
            const PkModelCollection* _modelCollection,
            std::map<AnalyteGroupId, std::shared_ptr<PkModel> >& _pkModel);

protected:
    bool findFormulationAndRoutes(
            std::vector<FormulationAndRoute>& _treatmentFandR,
            const FormulationAndRoutes& _drugModelFandR,
            std::map<FormulationAndRoute, const FullFormulationAndRoute*>& _result);

    Tucuxi::Common::LoggerHelper m_logger;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_GENERALEXTRACTOR_H
