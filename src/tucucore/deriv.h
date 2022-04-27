//@@lisence@@

#ifndef TUCUXI_CORE_DERIV_H
#define TUCUXI_CORE_DERIV_H

#include <Eigen/Dense>
#include <boost/type_traits.hpp>
#include <boost/type_traits/remove_pointer.hpp>

#include "tucucommon/general.h"

#include "definitions.h"

namespace Tucuxi {
namespace Core {

constexpr Value DEFAULT_DERIV_TOL = 2e-5;


/// Calculates derivatives for jacobian and hessians.
/// Uses type traits to select the code paths.
/// STL vectors and simple_matrix_t, or Eigen Vectors and Matrices are acceptable input.
/// Usees the default tolerance given as DEFAULT_DERIV_TOL to determine the value of h


///////////////////////////////////////////////////////////////////////////////
// !!!! START OF FIRST DERIVATIVE
///////////////////////////////////////////////////////////////////////////////

/// General case specialization of first derivatives for resolving typetraits
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implementeed operator()
/// \tparam x the type of dependent variable(s)
///
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param is_ptr whether x has pointer type
/// \param is_array whether x has array type
/// \param is_class whether x has class type
/// \param tol value of derivative calculation step (as h in f(x+h)...)
///
/// \return x derivative
template<typename func, typename x = double, bool is_ptr, bool is_array, bool is_class>
inline x deriv1_impl(
        func fxn, // NOLINT(readability-identifier-naming)
        x loc,    // NOLINT(readability-identifier-naming)
        const boost::integral_constant<bool, is_ptr>&,
        const boost::integral_constant<bool, is_array>&,
        const boost::integral_constant<bool, is_class>&,
        const Value tol = DEFAULT_DERIV_TOL) // NOLINT(readability-identifier-naming)
{
    return (fxn(loc + tol) - fxn(loc - tol)) / (2 * tol);
}


/// General case specialization for is pointer, not array, not class
/// of first derivatives after typetraits resolved
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implemented operator()
/// \tparam x the type of dependent variable(s)
///
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param is_ptr whether x has pointer type
/// \param is_array whether x has array type
/// \param is_class whether x has class type
/// \param tol value of derivative calculation step (as h in f(x+h)...)
///
/// \return x derivative
template<typename func, typename x>
inline x deriv1_impl(
        func fxn, // NOLINT(readability-identifier-naming)
        x loc,    // NOLINT(readability-identifier-naming)
        const boost::true_type&,
        const boost::false_type&,
        const boost::false_type&,
        const Value tol = DEFAULT_DERIV_TOL) // NOLINT(readability-identifier-naming)
{
    auto xp = *loc + tol;
    auto xm = *loc - tol;
    auto ret = (fxn(&xp) - fxn(&xm)) / (2 * tol);
    // Changed the return type
    // return &ret;
    return ret;
}



/// Partial specialization for the vector case not pointer, not array, is class type
/// of first derivatives after typetraits resolved
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implemented operator()
/// \tparam x the type of dependent variable(s)
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param is_ptr whether x has pointer type
/// \param is_array whether x has array type
/// \param is_class whether x has class type
/// \param answer where to put the results
/// \param tol value of derivative calculation step (as h in f(x+h)...)
/// \return x& derivative
template<typename func, typename x = ValueVector>
inline x& deriv1_impl(
        x& ans,   // NOLINT(readability-identifier-naming)
        func fxn, // NOLINT(readability-identifier-naming)
        x& loc,   // NOLINT(readability-identifier-naming)
        const boost::false_type&,
        const boost::false_type&,
        const boost::true_type&,
        const Value tol = DEFAULT_DERIV_TOL) // NOLINT(readability-identifier-naming)
{

    size_t vecsize = loc.size();

    x xm;
    x xp;
    xm = xp = loc;
    for (size_t i = 0; i < vecsize; ++i) {
        xp[i] = loc[i] + tol;
        xm[i] = loc[i] - tol;
        ans[i] = (fxn(xp) - fxn(xm)) / (2 * tol);
        xp[i] = loc[i];
        xm[i] = loc[i];
    }
    return ans;
}


/// Partial specialization for the vector case is pointer, not array, is class type
/// of first derivatives after typetraits resolved
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implemented operator()
/// \tparam x the type of dependent variable(s)
///
/// \param ans the resulting derivative
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param is_ptr whether x has pointer type
/// \param is_array whether x has array type
/// \param is_class whether x has class type
/// \param tol value of derivative calculation step (as h in f(x+h)...)
///
/// \return x& derivative
template<typename func, typename x = ValueVector>
inline x& deriv1_impl(
        x& ans,   // NOLINT(readability-identifier-naming)
        func fxn, // NOLINT(readability-identifier-naming)
        x& loc,   // NOLINT(readability-identifier-naming)
        const boost::true_type&,
        const boost::false_type&,
        const boost::true_type&,
        const Value tol = DEFAULT_DERIV_TOL) // NOLINT(readability-identifier-naming)
{

    int vecsize = loc.size();

    typedef typename boost::remove_pointer<x> basex;
    basex xm;
    basex xp;
    xm = xp = loc;
    for (int i = 0; i < vecsize; ++i) {
        xp[i] = loc[i] + tol;
        xm[i] = loc[i] - tol;
        ans[i] = (fxn(xp) - fxn(xm)) / (2 * tol);
        xp[i] = loc[i];
        xm[i] = loc[i];
    }
    return ans;
}


/// The following is a part of first derivatives that would actually be called from outside.
/// The methods above are for choosing implementations for different types of inputs
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implemented operator()
/// \tparam x the type of dependent variable(s)
///
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param ans the resulting derivative
/// \param tol value of derivative calculation step (as h in f(x+h)...)
///
/// \return x derivative
template<typename func, typename x = ValueVector>
inline x deriv1(func fxn, x& loc, x& ans, const Value tol = DEFAULT_DERIV_TOL) // NOLINT(readability-identifier-naming)
{
    TMP_UNUSED_PARAMETER(tol);
    // Using boost typetraits here to better handle typical specializations
    return deriv1_impl(ans, fxn, loc, boost::is_pointer<x>(), boost::is_array<x>(), boost::is_class<x>());
}


/// The following is a part of first derivatives that would actually be called from outside.
/// The methods above are for choosing implementations for different types of inputs
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implemented operator()
/// \tparam x the type of dependent variable(s)
///
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param tol value of derivative calculation step (as h in f(x+h)...)
///
/// \return x derivative
template<typename func, typename x = ValueVector>
inline x deriv1(func fxn, x& loc, const Value tol = DEFAULT_DERIV_TOL) // NOLINT(readability-identifier-naming)
{
    TMP_UNUSED_PARAMETER(tol);
    // Using boost typetraits here to better handle typical specializations
    return deriv1_impl(fxn, loc, boost::is_pointer<x>(), boost::is_array<x>(), boost::is_class<x>());
}

///////////////////////////////////////////////////////////////////////////////
// !!!! END OF FIRST DERIVATIVE
///////////////////////////////////////////////////////////////////////////////

/// General case specialization of second derivatives
/// \tparam func the objective function type for the derivatecalculations (f(x)).
/// 	Must have implementeed operator()
/// \tparam x the type of dependent variable(s)
/// \tparam y the type of the result
///
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param answer where to put the results
/// \param tol value of derivative calculation step (as h in f(x+h)...)
template<typename func, typename x, typename y>
void deriv2_impl(func fxn, x& loc, y& answer, /*const*/ Value tol)
{ // NOLINT(readability-identifier-naming)
    int size = loc.size();

    // Diagonal elements
    x xm;
    x xp;
    for (int i = 0; i < size; ++i) {
        xm = xp = loc;
        xp[i] = loc[i] + tol;
        xm[i] = loc[i] - tol;
        answer[i * size + i] = (fxn(xp) + fxn(xm) - 2 * fxn(loc)) / std::pow(tol, 2);
    }

    // Off-diagonal elements
    if (size > 1) {
        x xpp;
        x xmm;
        x xpm;
        x xmp;
        for (int i = 0; i < size - 1; ++i) {
            for (int j = i + 1; j < size; ++j) {
                xpp = xmm = xpm = xmp = loc;
                xpp[i] = loc[i] + tol;
                xpp[j] = loc[j] + tol;
                xmm[i] = loc[i] - tol;
                xmm[j] = loc[j] - tol;
                xpm[i] = loc[i] + tol;
                xpm[j] = loc[j] - tol;
                xmp[i] = loc[i] - tol;
                xmp[j] = loc[j] + tol;
                answer[i * size + j] = answer[j * size + i] =
                        (fxn(xpp) + fxn(xmm) - fxn(xmp) - fxn(xpm)) / (4 * std::pow(tol, 2));
            }
        }
    }
}


/// Partial specialization for the Eigen VectorXd/MatrixXd case of second derivatives
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implementeed operator()
///
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param answer where to put the results
/// \param tol value of derivative calculation step (as h in f(x+h)...)
template<typename func>
void deriv2_impl(func fxn, Eigen::VectorXd& loc, Eigen::MatrixXd& answer, const Value tol)
{ // NOLINT(readability-identifier-naming)
    long size = loc.size();

    // Diagonal elements
    for (long i = 0; i < size; ++i) {
        Eigen::VectorXd xm;
        Eigen::VectorXd xp;
        xm = xp = loc;
        xp(i) = loc(i) + tol;
        xm(i) = loc(i) - tol;
        answer(i, i) = (fxn(xp) + fxn(xm) - 2 * fxn(loc)) / std::pow(tol, 2);
    }

    // Off-diagonal elements
    if (size > 1) {
        for (long i = 0; i < size - 1; ++i) {
            for (long j = i + 1; j < size; ++j) {
                Eigen::VectorXd xpp;
                Eigen::VectorXd xmm;
                Eigen::VectorXd xpm;
                Eigen::VectorXd xmp;
                xpp = xmm = xpm = xmp = loc;
                xpp(i) = loc(i) + tol;
                xpp(j) = loc(j) + tol;
                xmm(i) = loc(i) - tol;
                xmm(j) = loc(j) - tol;
                xpm(i) = loc(i) + tol;
                xpm(j) = loc(j) - tol;
                xmp(i) = loc(i) - tol;
                xmp(j) = loc(j) + tol;
                answer(i, j) = answer(j, i) = (fxn(xpp) + fxn(xmm) - fxn(xmp) - fxn(xpm)) / (4 * std::pow(tol, 2));
            }
        }
    }
}

/// The following is the part of second derivatives that would actually be called from outside.
/// The methods above are for choosing implementations for different types of inputs
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implementeed operator()
/// \tparam x the type of dependent variable(s)
/// \tparam y the type of the result
///
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param ret where to put the results
/// \param tol value of derivative calculation step (as h in f(x+h)...)
template<typename func, typename x = ValueVector, typename y>
inline void deriv2(
        func fxn, x& loc, y& ret, /*const*/ Value tol = DEFAULT_DERIV_TOL) // NOLINT(readability-identifier-naming)
{
    // Using boost typetraits here to better handle typical specializations

    // If you want to inspect the types, I found uncommenting the following lines useful,
    //    bool lala1 = boost::is_pointer<x>();
    //    bool lala2 = boost::is_array<x>();
    //    bool lala3 = boost::is_class<x>();

    deriv2_impl(fxn, loc, ret, tol);
}

/// The following is the part of second derivatives that would actually be called from outside.
/// The methods above are for choosing implementations for different types of inputs
/// \tparam func the objective function type for the derivatecalculations (f(x)).
///  Must have implemented operator()
///
/// \param fxn object of func (f(x))
/// \param loc where to take the derivative
/// \param ret where to put the results
/// \param tol value of derivative calculation step (as h in f(x+h)...)
template<typename func>
inline void deriv2(
        func fxn,
        Eigen::VectorXd& loc,
        Eigen::MatrixXd& ret,
        const Value tol = DEFAULT_DERIV_TOL) // NOLINT(readability-identifier-naming)
{
    deriv2_impl(fxn, loc, ret, tol);
}


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_
