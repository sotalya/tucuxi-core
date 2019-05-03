#ifndef TUCUXI_CORE_RK4TWOCOMPARTMENTERLANG4_H
#define TUCUXI_CORE_RK4TWOCOMPARTMENTERLANG4_H

#include "tucucore/intakeintervalcalculatorrk4.h"


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
///struct copy {
///    static inline void apply(const Residuals& _inResiduals,  std::vector<double> &_concentrations) {
///        _concentrations[from] = _inResiduals[from];
///        copy<from+1,to>:: apply(_inResiduals, _concentrations);
///    }
///
///    static inline void update(double _ktr, const std::vector<double> &_c, std::vector<double>& _dcdt) {
///        _dcdt[from] =  _ktr * _c[from - 1] - _ktr * _c[from];
///        copy<from+1,to>:: update(_ktr, _c, _dcdt);
///    }
///};
///
///// Terminal case
///template<int from>
///struct copy<from, from> {
///    static inline void apply(const Residuals& _inResiduals,  std::vector<double> &_concentrations) {
///        _concentrations[from] = _inResiduals[from];
///    }
///
///    static inline void update(double _ktr, const std::vector<double> &_c, std::vector<double>& _dcdt) {
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
///    copy<0,2>::apply(r, c);
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
struct copy {
    static inline void apply(const Residuals& _inResiduals,  std::vector<double> &_concentrations) {
        _concentrations[from] = _inResiduals[from];
        copy<from+1,to>:: apply(_inResiduals, _concentrations);
    }

    static inline void update(Value _ktr, const std::vector<double> &_c, std::vector<double>& _dcdt) {
        _dcdt[from] =  _ktr * _c[from - 1] - _ktr * _c[from];
        copy<from+1,to>:: update(_ktr, _c, _dcdt);
    }
};

// Terminal case
template<int from>
struct copy<from, from> {
    static inline void apply(const Residuals& _inResiduals,  std::vector<double> &_concentrations) {
        _concentrations[from] = _inResiduals[from];
    }

    static inline void update(Value _ktr, const std::vector<double> &_c, std::vector<double>& _dcdt) {
        _dcdt[from] =  _ktr * _c[from - 1] - _ktr * _c[from];
    }
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the 2-compartment with Erlang absorption (4) algorithm
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
///
class RK4TwoCompartmentErlang4Micro : public IntakeIntervalCalculatorRK4Base<7, RK4TwoCompartmentErlang4Micro>
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4TwoCompartmentErlang4Micro)
public:
    /// \brief Constructor
    RK4TwoCompartmentErlang4Micro();

    enum class CompartmentsEnum : int { Central = 0, Peripheral, Dose, A2, A3, A4, A5 };

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations (const Residuals& _inResiduals, std::vector<double> &_concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2] + m_D / m_V1;
        copy<3,6>::apply(_inResiduals, _concentrations);
//        _concentrations[3] = _inResiduals[3];
//        _concentrations[4] = _inResiduals[4];
//        _concentrations[5] = _inResiduals[5];
//        _concentrations[6] = _inResiduals[6];
    }

public:
    inline void derive(double _t, const std::vector<double> &_c, std::vector<double>& _dcdt)
    {
        FINAL_UNUSED_PARAMETER(_t);
        _dcdt[0] = m_Ktr * _c[6] - m_Ke * _c[0] + m_K21 * _c[1] - m_K12 * _c[0];
        _dcdt[1] = m_K12 * _c[0] - m_K21 * _c[1];
        _dcdt[2] = - m_Ktr * _c[2];
        copy<3,6>::update(m_Ktr, _c, _dcdt);
//        _dcdt[3] = m_Ktr * _c[2] - m_Ktr * _c[3];
//        _dcdt[4] = m_Ktr * _c[3] - m_Ktr * _c[4];
//        _dcdt[5] = m_Ktr * _c[4] - m_Ktr * _c[5];
//        _dcdt[6] = m_Ktr * _c[5] - m_Ktr * _c[6];
    }

protected:


    Value m_D;	/// Quantity of drug
    Value m_F;  /// Biodisponibility
    Value m_V1;  /// Volume of the central compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_K12; /// Inter-compartment rate between central and peripheral
    Value m_K21; /// Inter-compartment rate between peripheral and central
    Value m_Ktr; /// Transit compartments constant rate

private:
    typedef CompartmentsEnum Compartments;
};



class RK4TwoCompartmentErlang4Macro : public RK4TwoCompartmentErlang4Micro
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4TwoCompartmentErlang4Macro)
public:
    RK4TwoCompartmentErlang4Macro();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RK4TWOCOMPARTMENTERLANG4_H
