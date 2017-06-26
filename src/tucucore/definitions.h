/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_DEFINITIONS_H
#define TUCUXI_MATH_DEFINITIONS_H

#include <vector>
#include <map>

#include "Eigen/Dense"

namespace Tucuxi {
namespace Core {

typedef double Float;					// General decimal number in math module (use this to set the precision for all number values)
typedef Float Time;				// Used to set precision of time variables
typedef Float Concentration;	// Used for 0-D concentration

typedef std::vector<Time> Times;					//used for 1-D concentrations
typedef std::vector<Concentration> Concentrations;	//used for 1-D concentrations

typedef std::vector<Concentration> Residuals;		//used for 1-D residuals

typedef int CycleSize; //used as the number of points in the cycle.

typedef std::map<std::string, Eigen::VectorXd> PrecomputedLogarithms; 

}
}

#endif // TUCUXI_MATH_DEFINITIONS_H