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


#ifndef MINIMIZE_H
#define MINIMIZE_H

#include "tucucommon/general.h"

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

// NOLINTBEGIN(readability-identifier-naming)

/// See Numerical Recipes https://www.uam.es/personal_pdi/ciencias/ppou/CNC/TEMA6/f10.pdf
/// This is implemented in boost: #include <boost/math/tools/minima.hpp> where they also
/// cute Numerical Recipes
/// Should use that implementation its better as it can fall back to simper methods
/// For documentation just check out the link here: http://www.boost.org/doc/libs/1_60_0/libs/math/doc/html/math_toolkit/roots/brent_minima.html
struct Bracketmethod
{
    double ax, bx, cx, fa, fb, fc; // NOLINT(readability-isolate-declaration)
    template<class T>
    void bracket(const double a, const double b, T& func)
    {
        const double GOLD = 1.618034;
        const double GLIMIT = 100.0;
        const double TINY = 1.0e-20;
        ax = a;
        bx = b;
        double fu;
        fa = func(ax);
        fb = func(bx);
        if (fb > fa) {
            std::swap(ax, bx);
            std::swap(fb, fa);
        }
        cx = bx + GOLD * (bx - ax);
        fc = func(cx);
        while (fb > fc) {
            double r = (bx - ax) * (fb - fc);
            double q = (bx - cx) * (fb - fa);
            double u = bx - ((bx - cx) * q - (bx - ax) * r) / (2.0 * copysign(std::max(std::abs(q - r), TINY), q - r));
            double ulim = bx + GLIMIT * (cx - bx);
            if ((bx - u) * (u - cx) > 0.0) {
                fu = func(u);
                if (fu < fc) {
                    ax = bx;
                    bx = u;
                    fa = fb;
                    fb = fu;
                    return;
                }
                // Do not use else after return
                if (fu > fb) {
                    cx = u;
                    fc = fu;
                    return;
                }
                u = cx + GOLD * (cx - bx);
                fu = func(u);
            }
            else if ((cx - u) * (u - ulim) > 0.0) {
                fu = func(u);
                if (fu < fc) {
                    shft3(bx, cx, u, u + GOLD * (u - cx));
                    shft3(fb, fc, fu, func(u));
                }
            }
            else if ((u - ulim) * (ulim - cx) >= 0.0) {
                u = ulim;
                fu = func(u);
            }
            else {
                u = cx + GOLD * (cx - bx);
                fu = func(u);
            }
            shft3(ax, bx, cx, u);
            shft3(fa, fb, fc, fu);
        }
    }

    inline void shft2(double& a, double& b, const double c)
    {
        a = b;
        b = c;
    }
    inline void shft3(double& a, double& b, double& c, const double d)
    {
        a = b;
        b = c;
        c = d;
    }
    inline void mov3(double& a, double& b, double& c, const double d, const double e, const double f)
    {
        a = d;
        b = e;
        c = f;
    }
};

struct Dbrent : Bracketmethod
{
    double xmin, fmin;
    const double tol;
    Dbrent(const double toll = 3.0e-8) : tol(toll) {} // NOLINT(cppcoreguidelines-pro-type-member-init)
    template<class T>
    double minimize(T& funcd)
    {
        const int ITMAX = 100;
        const double ZEPS = std::numeric_limits<double>::epsilon() * 1.0e-3;
        bool ok1, ok2;                                                   // NOLINT(readability-isolate-declaration)
        double a, b, d = 0.0, d1, d2, du, dv, dw, dx, e = 0.0;           // NOLINT(readability-isolate-declaration)
        double fu, fv, fw, fx, olde, tol1, tol2, u, u1, u2, v, w, x, xm; // NOLINT(readability-isolate-declaration)
        a = (ax < cx ? ax : cx);
        b = (ax > cx ? ax : cx);
        x = w = v = bx;
        fw = fv = fx = funcd(x);
        dw = dv = dx = funcd.df(x);
        for (int iter = 0; iter < ITMAX; iter++) {
            xm = 0.5 * (a + b);
            tol1 = tol * std::abs(x) + ZEPS;
            tol2 = 2.0 * tol1;
            if (std::abs(x - xm) <= (tol2 - 0.5 * (b - a))) {
                fmin = fx;
                return xmin = x;
            }
            if (std::abs(e) > tol1) {
                d1 = 2.0 * (b - a);
                d2 = d1;
                if (dw != dx) {
                    d1 = (w - x) * dx / (dx - dw);
                }
                if (dv != dx) {
                    d2 = (v - x) * dx / (dx - dv);
                }
                u1 = x + d1;
                u2 = x + d2;
                ok1 = (a - u1) * (u1 - b) > 0.0 && dx * d1 <= 0.0;
                ok2 = (a - u2) * (u2 - b) > 0.0 && dx * d2 <= 0.0;
                olde = e;
                e = d;
                if (ok1 || ok2) {
                    if (ok1 && ok2) {
                        d = (std::abs(d1) < std::abs(d2) ? d1 : d2);
                    }
                    else if (ok1) {
                        d = d1;
                    }
                    else {
                        d = d2;
                    }
                    if (std::abs(d) <= std::abs(0.5 * olde)) {
                        u = x + d;
                        if (u - a < tol2 || b - u < tol2) {
                            d = copysign(tol1, xm - x);
                        }
                    }
                    else {
                        d = 0.5 * (e = (dx >= 0.0 ? a - x : b - x));
                    }
                }
                else {
                    d = 0.5 * (e = (dx >= 0.0 ? a - x : b - x));
                }
            }
            else {
                d = 0.5 * (e = (dx >= 0.0 ? a - x : b - x));
            }
            if (std::abs(d) >= tol1) {
                u = x + d;
                fu = funcd(u);
            }
            else {
                u = x + copysign(tol1, d);
                fu = funcd(u);
                if (fu > fx) {
                    fmin = fx;
                    return xmin = x;
                }
            }
            du = funcd.df(u);
            if (fu <= fx) {
                if (u >= x) {
                    a = x;
                }
                else {
                    b = x;
                }
                mov3(v, fv, dv, w, fw, dw);
                mov3(w, fw, dw, x, fx, dx);
                mov3(x, fx, dx, u, fu, du);
            }
            else {
                if (u < x) {
                    a = u;
                }
                else {
                    b = u;
                }
                if (fu <= fw || w == x) {
                    mov3(v, fv, dv, w, fw, dw);
                    mov3(w, fw, dw, u, fu, du);
                }
                else if (fu < fv || v == x || v == w) {
                    mov3(v, fv, dv, u, fu, du);
                }
            }
        }
        return xmin = x;
        //        throw("Too many iterations in routine dbrent");
    }
};

template<typename T>
struct Df1dim
{
    const ValueVector& p;
    const ValueVector& xi;
    size_t n;
    T& funcd;
    ValueVector xt;
    ValueVector dft;
    Df1dim(ValueVector& pp, ValueVector& xii, T& funcdd) : p(pp), xi(xii), n(pp.size()), funcd(funcdd), xt(n), dft(n) {}
    double operator()(const Value x)
    {
        for (size_t j = 0; j < n; j++) {
            xt[j] = p[j] + x * xi[j];
        }
        return funcd(xt);
    }
    double df(const Value x)
    {
        TMP_UNUSED_PARAMETER(x);
        double df1 = 0.0;
        funcd.df(xt, dft);
        for (size_t j = 0; j < n; j++) {
            df1 += dft[j] * xi[j];
        }
        return df1;
    }
};

template<typename T>
struct Dlinemethod
{
    ValueVector p;
    ValueVector xi;
    T& func;
    size_t n;
    Dlinemethod(T& funcc) : func(funcc) {} // NOLINT(cppcoreguidelines-pro-type-member-init)
    double linmin()
    {
        double ax, xx, xmin; // NOLINT(readability-isolate-declaration)
        n = p.size();
        Df1dim<T> df1dim(p, xi, func);
        ax = 0.0;
        xx = 1.0;
        Dbrent dbrent;
        dbrent.bracket(ax, xx, df1dim);
        xmin = dbrent.minimize(df1dim);
        for (size_t j = 0; j < n; j++) {
            xi[j] *= xmin;
            p[j] += xi[j];
        }
        return dbrent.fmin;
    }
};

template<typename T>
struct Frprmn : Dlinemethod<T>
{
    //int iter;
    double fret;
    using Dlinemethod<T>::func;
    using Dlinemethod<T>::linmin;
    using Dlinemethod<T>::p;
    using Dlinemethod<T>::xi;
    const double ftol;
    Frprmn(T& funcd, const double ftoll = 3.0e-8)
        : Dlinemethod<T>(funcd), // NOLINT(cppcoreguidelines-pro-type-member-init)
          ftol(ftoll)
    {
    }

    ValueVector minimize(const ValueVector& pp)
    {
        const int ITMAX = 200;
        const double EPS = 1.0e-18;
        const double GTOL = 1.0e-8;
        double gg, dgg; // NOLINT(readability-isolate-declaration)
        size_t n = pp.size();
        p = pp;
        ValueVector g(n), h(n); // NOLINT(readability-isolate-declaration)
        xi.resize(n);
        double fp = func(p);
        func.df(p, xi);

        for (size_t j = 0; j < n; j++) {
            g[j] = -xi[j];
            xi[j] = h[j] = g[j];
        }

        bool equal = false;
        for (int its = 0; its < ITMAX; its++) {
            fret = linmin();

            // The following lines are here to detect if we are at the end of the minimization
            if (equal) {
                if (2.0 * std::abs(fret - fp) <= ftol * (std::abs(fret) + std::abs(fp) + EPS)) {
                    return p;
                }
            }
            if (fret == fp) {
                equal = true;
            }
            else {
                equal = false;
            }

            fp = fret;
            func.df(p, xi);

            double test = 0.0;
            double den = std::max(fp, 1.0);

            for (size_t j = 0; j < n; j++) {
                double temp = std::abs(xi[j]) * std::max(std::abs(p[j]), Value(1.0)) / den;
                if (temp > test) {
                    test = temp;
                }
            }

            if (test < GTOL) {
                return p;
            }

            dgg = gg = 0.0;

            for (size_t j = 0; j < n; j++) {
                gg += g[j] * g[j];
                //                dgg += xi[j]*xi[j]; //This statement for Fletcher-Reeves.
                dgg += (xi[j] + g[j]) * xi[j]; //This statement for Polak-Ribiere.
            }

            if (gg == 0.0) {
                return p;
            }

            double gam = dgg / gg;
            for (size_t j = 0; j < n; j++) {
                g[j] = -xi[j];
                xi[j] = h[j] = g[j] + gam * h[j];
            }
        }
        return p;
        //        throw("Too many iterations in frprmn");
    }
};

// NOLINTEND(readability-identifier-naming)

} // namespace Core
} // namespace Tucuxi

#endif // MINIMIZE_H
