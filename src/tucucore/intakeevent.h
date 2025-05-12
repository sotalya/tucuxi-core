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


#ifndef TUCUXI_CORE_INTAKEEVENT_H
#define TUCUXI_CORE_INTAKEEVENT_H

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeintervalcalculator.h"
#include "tucucore/timedevent.h"

using Tucuxi::Common::DateTime; // NOLINT(google-global-names-in-headers)
using Tucuxi::Common::Duration; // NOLINT(google-global-names-in-headers)

namespace Tucuxi {
namespace Core {

class IntakeIntervalCalculator;

/// \ingroup TucuCore
/// \brief A class representing the event of taking a dose.
/// Represents a Dose, as extracted from a Dosage history.
class IntakeEvent : public TimedEvent
{
public:
    /// \brief The default constructor is not needed
    IntakeEvent() = delete;

    /// \brief Constructor
    /// \param _time Time of the dose intake.
    /// \param _offsetTime Number of hours since the first dose.
    /// \param _dose Quantity of drug administered.
    /// \param _interval Time before the next intake.
    /// \param _formulationAndRoute Formulation and route of administration.
    /// \param _infusionTime Duration in case of an infusion.
    /// \param _nbPoints Number of points to compute for this intake.
    IntakeEvent(
            DateTime _time,
            Duration _offsetTime,
            DoseValue _dose,
            TucuUnit _doseUnit,
            Duration _interval,
            FormulationAndRoute _formulationAndRoute,
            Duration _infusionTime,
            CycleSize _nbPoints)
        : TimedEvent(_time), m_dose(_dose), m_doseUnit(std::move(_doseUnit)), m_offsetTime(_offsetTime),
          m_formulationAndRoute(std::move(_formulationAndRoute)), m_absorptionModel(AbsorptionModel::Undefined),
          m_interval(_interval), m_infusionTime(_infusionTime), m_calculator(nullptr)
    {
        // YTA : I don't get why we should have odd numbers...
        m_nbPoints = _nbPoints; // % 2 != 0 ? _nbPoints : _nbPoints + 1;  // Must use an odd number

        if (_interval.isNegative()) {
            std::cout << "Negative interval" << '\n';
        }

        if (_interval.isEmpty()) {
            std::cout << "Zero interval" << '\n';
        }
    }


    /// \brief Constructor
    /// \param _time Time of the dose intake.
    /// \param _offsetTime Number of hours since the first dose.
    /// \param _dose Quantity of drug administered.
    /// \param _interval Time before the next intake.
    /// \param _formulationAndRoute Formulation and route of administration.
    /// \param _absorptionModel Absorption model of the administration.
    /// \param _infusionTime Duration in case of an infusion.
    /// \param _nbPoints Number of points to compute for this intake.
    ///
    /// This constructor is meant to be used for tests, not by the software itself.
    ///
    IntakeEvent(
            DateTime _time,
            Duration _offsetTime,
            DoseValue _dose,
            TucuUnit _doseUnit,
            Duration _interval,
            FormulationAndRoute _formulationAndRoute,
            AbsorptionModel _absorptionModel,
            Duration _infusionTime,
            CycleSize _nbPoints)
        : TimedEvent(_time), m_dose(_dose), m_doseUnit(std::move(_doseUnit)), m_offsetTime(_offsetTime),
          m_formulationAndRoute(std::move(_formulationAndRoute)), m_absorptionModel(_absorptionModel),
          m_interval(_interval), m_infusionTime(_infusionTime), m_calculator(nullptr)
    {
        // YTA : I don't get why we should have odd numbers...
        m_nbPoints = _nbPoints; // % 2 != 0 ? _nbPoints : _nbPoints + 1;  // Must use an odd number

        if (_interval.isNegative()) {
            std::cout << "Negative interval" << '\n';
        }

        if (_interval.isEmpty()) {
            std::cout << "Zero interval" << '\n';
        }
    }

    /// \brief Destructor
    ~IntakeEvent() {}

    /// \brief Default copy constructor.
    /// \param _other Source object to copy from.
    IntakeEvent(const IntakeEvent& _other) = default;

    /// \brief Default assignment operator.
    /// \param _other Source object to copy from.
    IntakeEvent& operator=(const IntakeEvent& _other) = default;

    /// \brief Comparison operator.
    /// \param _other Object to compare to.
    /// \return true if the two objects are equal, false otherwise.
    bool operator==(const IntakeEvent& _other) const
    {
        return (m_time == _other.m_time && m_dose == _other.m_dose && m_offsetTime == _other.m_offsetTime
                && m_nbPoints == _other.m_nbPoints && m_absorptionModel == _other.m_absorptionModel
                && m_interval == _other.m_interval && m_infusionTime == _other.m_infusionTime);
    }

    /// \brief Change the time before the next intake.
    /// \param _interval New interval size.
    void setInterval(Duration _interval)
    {
        if (_interval.isNegative()) {
            std::cout << "Bad interval" << '\n';
        }
        m_interval = _interval;
    }

    /// \brief Get the interval value in milliseconds.
    /// \return Interval value in ms.
    Duration getInterval() const
    {
        return m_interval;
    }

    /// \brief Change the number of points to compute for this intake
    /// \param _interval New number of points
    void setNbPoints(CycleSize _nbPoints)
    {
        m_nbPoints = _nbPoints;
    }

    /// \brief Get the number of points to compute for this intake
    /// \return Number of points to compute for this intake
    CycleSize getNbPoints() const
    {
        return m_nbPoints;
    }

    /// \brief Set the dose.
    /// \param Dose value.
    void setDose(DoseValue _dose)
    {
        m_dose = _dose;
    }

    /// \brief Get the dose.
    /// \return Dose value.
    DoseValue getDose() const
    {
        return m_dose;
    }

    /// \brief Get the dose unit
    /// \return The dose unit
    TucuUnit getUnit() const
    {
        return m_doseUnit;
    }

    /// \brief Get the time (in milliseconds) since the start of the treatment.
    /// \return Time in ms since the beginning of the treatment.
    Duration getOffsetTime() const
    {
        return m_offsetTime;
    }

    ///
    /// \brief Get the formulation and route of administration of the treatment
    /// \return The formulation and route
    FormulationAndRoute getFormulationAndRoute() const
    {
        return m_formulationAndRoute;
    }

    /// \brief Get the absorption model of administration of the treatment.
    /// \return Absorption model of administration.
    AbsorptionModel getAbsorptionModel() const
    {
        return m_absorptionModel;
    }

    /// \brief Get the infusion time (in milliseconds).
    /// \return Infusion time (in ms).
    Duration getInfusionTime() const
    {
        return m_infusionTime;
    }

    void setAbsorptionModel(AbsorptionModel _model)
    {
        m_absorptionModel = _model;
    }

    /// \brief Compare two intakes, sorting them according to intake time.
    /// \param _other The intake to compare to.
    /// \return True if the current IntakeEvent is relative to an intake that precedes the one is compared to, false
    /// otherwise.
    bool operator<(const IntakeEvent& _other) const
    {
        return m_time < _other.m_time;
    }

    ///
    /// \brief setCalculator Defines the calculator to be used for calculation
    /// \param _calculator The calculator itself
    void setCalculator(std::shared_ptr<IntakeIntervalCalculator> _calculator)
    {
        m_calculator = std::move(_calculator);
    }

    std::shared_ptr<IntakeIntervalCalculator> getCalculator() const
    {
        return m_calculator;
    }

    ComputingStatus calculateIntakePoints(
            MultiCompConcentrations& _concentrations,
            TimeOffsets& _times,
            const IntakeEvent& _intakeEvent,
            const ParameterSetEvent& _parameters,
            const Residuals& _inResiduals,
            bool _isAll,
            Residuals& _outResiduals,
            const bool _isDensityConstant) const
    {
        return m_calculator->calculateIntakePoints(
                _concentrations,
                _times,
                _intakeEvent,
                _parameters,
                _inResiduals,
                _isAll,
                _outResiduals,
                _isDensityConstant);
    }

    ComputingStatus calculateIntakeSinglePoint(
            MultiCompConcentrations& _concentrations,
            const IntakeEvent& _intakeEvent,
            const ParameterSetEvent& _parameters,
            const Residuals& _inResiduals,
            const Value& _atTime,
            bool _isAll,
            Residuals& _outResiduals) const
    {
        return m_calculator->calculateIntakeSinglePoint(
                _concentrations, _intakeEvent, _parameters, _inResiduals, _atTime, _isAll, _outResiduals);
    }

private:
    /// The dose value
    DoseValue m_dose;
    /// The unit of the dose
    TucuUnit m_doseUnit;
    /// Number of hours since the first dose
    Duration m_offsetTime;
    /// Number of points to compute for this intake
    CycleSize m_nbPoints;
    /// The formulation and route of this intake
    FormulationAndRoute m_formulationAndRoute;
    /// The route of administration
    AbsorptionModel m_absorptionModel;
    /// The time before the next intake
    Duration m_interval;
    /// The duration in case of an infusion
    Duration m_infusionTime;

    std::shared_ptr<IntakeIntervalCalculator> m_calculator;
};


/// \brief An intake series is list of intake events.
/// This vector could be replaced in the future with a class that offers more capabilities (for instance, it could
/// incorporate the time interval spanned by the series). However, it will have to expose the same capabilities as the
/// underlying std::vector, thus the transition will be painless.
typedef std::vector<IntakeEvent> IntakeSeries;

typedef std::map<AnalyteGroupId, IntakeSeries> GroupsIntakeSeries;



void cloneIntakeSeries(const std::vector<IntakeEvent>& _input, std::vector<IntakeEvent>& _output);


void selectRecordedIntakes(
        IntakeSeries& _selectionSeries,
        const IntakeSeries& _intakeSeries,
        const DateTime& _recordFrom,
        const DateTime& _recordTo);

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_INTAKEEVENT_H
