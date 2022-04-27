//@@lisence@@

#ifndef TUCUXI_TUCUCOMMON_BASETYPES_H
#define TUCUXI_TUCUCOMMON_BASETYPES_H

#include <cmath>

typedef long long int int64;

typedef double ChronoBaseType;

// 3.14159;
static const double PI = acos(-1); // NOLINT(readability-identifier-naming)

// The following option would be good but generate some issues on Windows
// constexpr double PI = M_PI; // NOLINT(readability-identifier-naming)

#endif // TUCUXI_TUCUCOMMON_BASETYPES_H
