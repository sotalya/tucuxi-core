#ifndef TUCUXI_CORE_RK4TWOCOMPARTMENTERLANG4_H
#define TUCUXI_CORE_RK4TWOCOMPARTMENTERLANG4_H

#include "tucucore/intakeintervalcalculatorrk4.h"

#include "tucucore/intakeevent.h"


namespace Tucuxi {
namespace Core {

///
/// Template meta-programming to automate the calculation of all transit compartments.
/// It is a tentative to get a template class depending on the number of transit compartments.
///
/// Actually, the compiler does not need to inline the functions, so compiling with -O0 implies
/// the use of templates is less efficient than hardcoding these assignments.
/// However, compiling with -O2 makes both options similar. I have tested that with that example,
/// compiled like this:
///
/// g++ -DTEMPLATE -S -O2 test.cpp -o template.s
/// g++ -S -O2 test.cpp -o notemplate.s
///
/// \code
///#include <vector>
///#include <iostream>
///
///typedef std::vector<double> Residuals;
///
///template<int from, int to>
///struct TransitComps {
///    static inline void init(const Residuals& _inResiduals,  std::vector<double> &_concentrations) {
///        _concentrations[from] = _inResiduals[from];
///        TransitComps<from+1,to>:: apply(_inResiduals, _concentrations);
///    }
///
///    static inline void derive(double _ktr, const std::vector<double> &_c, std::vector<double>& _dcdt) {
///        _dcdt[from] =  _ktr * _c[from - 1] - _ktr * _c[from];
///        TransitComps<from+1,to>:: update(_ktr, _c, _dcdt);
///    }
///};
///
///// Terminal case
///template<int from>
///struct TransitComps<from, from> {
///    static inline void init(const Residuals& _inResiduals,  std::vector<double> &_concentrations) {
///        _concentrations[from] = _inResiduals[from];
///    }
///
///    static inline void derive(double _ktr, const std::vector<double> &_c, std::vector<double>& _dcdt) {
///        _dcdt[from] =  _ktr * _c[from - 1] - _ktr * _c[from];
///    }
///};
///int main()
///{
///    Residuals r(3);
///    Residuals c(3);
///
///    r[0] = 1.0;
///    r[1] = 2.0;
///    r[2] = 3.0;
///#ifdef TEMPLATE
///    TransitComps<0,2>::init(r, c);
///#else
///    c[0] = r[0];
///    c[1] = r[1];
///    c[2] = r[2];
///#endif
///    std::cout << c[0] << c[1] << c[2];
///return 0;
///}
/// \endcode
///
///
template<int from, int to>
struct TransitComps {
    static inline void init(const Residuals& _inResiduals,  std::vector<double> &_concentrations) {
        _concentrations[from] = _inResiduals[from];
        TransitComps<from+1,to>:: init(_inResiduals, _concentrations);
    }

    static inline void derive(Value _ktr, const std::vector<double> &_c, std::vector<double>& _dcdt) {
        _dcdt[from] =  _ktr * _c[from - 1] - _ktr * _c[from];
        TransitComps<from+1,to>:: derive(_ktr, _c, _dcdt);
    }
};

// Terminal case
template<int from>
struct TransitComps<from, from> {
    static inline void init(const Residuals& _inResiduals,  std::vector<double> &_concentrations) {
        _concentrations[from] = _inResiduals[from];
    }

    static inline void derive(Value _ktr, const std::vector<double> &_c, std::vector<double>& _dcdt) {
        _dcdt[from] =  _ktr * _c[from - 1] - _ktr * _c[from];
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the 2-compartment with Erlang absorption (N) algorithm
/// \sa IntakeIntervalCalculator
///
/// Compartments:
/// 0 : Central
/// 1 : Peripheral
/// 2 : Where the dose is injected
/// 3 : transit 1
/// 4 : transit 2
/// 5 : transit 3
/// 6 : transit 4
/// ...
///
/// The template parameter NbTransitCompartment allows to define the number of transit compartments.
///
template<int NbTransitCompartment>
class RK4TwoCompartmentErlangMicro : public IntakeIntervalCalculatorRK4Base<NbTransitCompartment + 3, RK4TwoCompartmentErlangMicro<NbTransitCompartment> >
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4TwoCompartmentErlangMicro<NbTransitCompartment>)
public:
    /// \brief Constructor
    RK4TwoCompartmentErlangMicro() : IntakeIntervalCalculatorRK4Base<NbTransitCompartment + 3, RK4TwoCompartmentErlangMicro<NbTransitCompartment> > (new PertinentTimesCalculatorStandard())
    {}

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId()
    {
        return {"V1", "Ktr", "Ke", "K12", "K21", "F"};
    }

    enum class CompartmentsEnum : int { Central = 0, Peripheral, Dose, A2, A3, A4, A5 };

protected:

    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override
    {
        if (!this->checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
            return false;
        }

        m_D = _intakeEvent.getDose();
        m_V1 = _parameters.getValue(ParameterId::V1);
        m_Ktr = _parameters.getValue(ParameterId::Ktr);
        m_Ke = _parameters.getValue(ParameterId::Ke);
        m_K12 = _parameters.getValue(ParameterId::K12);
        m_K21 = _parameters.getValue(ParameterId::K21);
        m_F = _parameters.getValue(ParameterId::F);

        this->m_nbPoints = _intakeEvent.getNbPoints();
        this->m_Int = (_intakeEvent.getInterval()).toHours();

        // check the inputs
        bool bOK = true;
        bOK &= this->checkPositiveValue(m_D, "The dose");
        bOK &= this->checkStrictlyPositiveValue(m_V1, "The volume");
        bOK &= this->checkStrictlyPositiveValue(m_F, "The bioavailability");
        bOK &= this->checkStrictlyPositiveValue(m_Ke, "The absorption constant");
        bOK &= this->checkStrictlyPositiveValue(m_Ktr, "The Ktr");
        bOK &= this->checkStrictlyPositiveValue(m_K12, "K12");
        bOK &= this->checkStrictlyPositiveValue(m_K21, "K21");
        bOK &= this->checkCondition(this->m_nbPoints > 0, "The number of points is zero or negative.");
        bOK &= this->checkCondition(this->m_Int > 0, "The interval time is negative.");

        return bOK;
    }


    void initConcentrations (const Residuals& _inResiduals, std::vector<double> &_concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2] + m_D / m_V1;
        TransitComps<3,3 + NbTransitCompartment - 1>::init(_inResiduals, _concentrations);
//        _concentrations[3] = _inResiduals[3];
//        _concentrations[4] = _inResiduals[4];
//        _concentrations[5] = _inResiduals[5];
//        _concentrations[6] = _inResiduals[6];
    }

public:
    inline void derive(double _t, const std::vector<double> &_c, std::vector<double>& _dcdt)
    {
        FINAL_UNUSED_PARAMETER(_t);
        _dcdt[0] = m_Ktr * _c[3 + NbTransitCompartment - 1] - m_Ke * _c[0] + m_K21 * _c[1] - m_K12 * _c[0];
        _dcdt[1] = m_K12 * _c[0] - m_K21 * _c[1];
        _dcdt[2] = - m_Ktr * _c[2];
        TransitComps<3,3 + NbTransitCompartment - 1>::derive(m_Ktr, _c, _dcdt);
//        _dcdt[3] = m_Ktr * _c[2] - m_Ktr * _c[3];
//        _dcdt[4] = m_Ktr * _c[3] - m_Ktr * _c[4];
//        _dcdt[5] = m_Ktr * _c[4] - m_Ktr * _c[5];
//        _dcdt[6] = m_Ktr * _c[5] - m_Ktr * _c[6];
    }

    inline void addFixedValue(double _t, std::vector<double>& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }

protected:


    Value m_D;	/// Quantity of drug
    Value m_F;  /// bioavailability
    Value m_V1;  /// Volume of the central compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_K12; /// Inter-compartment rate between central and peripheral
    Value m_K21; /// Inter-compartment rate between peripheral and central
    Value m_Ktr; /// Transit compartments constant rate

private:
    typedef CompartmentsEnum Compartments;
};




template<int NbTransitCompartment>
class RK4TwoCompartmentErlangMacro : public RK4TwoCompartmentErlangMicro<NbTransitCompartment>
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4TwoCompartmentErlangMacro)
public:
    RK4TwoCompartmentErlangMacro() : RK4TwoCompartmentErlangMicro<NbTransitCompartment>() {}

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId()
    {
        return {"V1", "V2", "Ktr", "CL", "Q", "F"};
    }

protected:

    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override
    {
        if (!this->checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
            return false;
        }


        this->m_D = _intakeEvent.getDose();
        this->m_V1 = _parameters.getValue(ParameterId::V1);
        Value v2 = _parameters.getValue(ParameterId::V2);
        this->m_Ktr = _parameters.getValue(ParameterId::Ktr);
        Value cl = _parameters.getValue(ParameterId::CL);
        Value q = _parameters.getValue(ParameterId::Q);
        this->m_F = _parameters.getValue(ParameterId::F);

        this->m_K12 = q / this->m_V1;
        this->m_K21 = q / v2;
        this->m_Ke = cl / this->m_V1;

        this->m_nbPoints = _intakeEvent.getNbPoints();
        this->m_Int = (_intakeEvent.getInterval()).toHours();

        // check the inputs
        bool bOK = true;
        bOK &= this->checkPositiveValue(this->m_D, "The dose");
        bOK &= this->checkStrictlyPositiveValue(this->m_V1, "The volume");
        bOK &= this->checkStrictlyPositiveValue(this->m_F, "The bioavailability");
        bOK &= this->checkStrictlyPositiveValue(this->m_Ke, "The absorption constant");
        bOK &= this->checkStrictlyPositiveValue(this->m_Ktr, "The Ktr");
        bOK &= this->checkStrictlyPositiveValue(this->m_K12, "K12");
        bOK &= this->checkStrictlyPositiveValue(this->m_K21, "K21");
        bOK &= this->checkCondition(this->m_nbPoints > 0, "The number of points is zero or negative.");
        bOK &= this->checkCondition(this->m_Int > 0, "The interval time is negative.");

        return bOK;
    }


};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RK4TWOCOMPARTMENTERLANG4_H
