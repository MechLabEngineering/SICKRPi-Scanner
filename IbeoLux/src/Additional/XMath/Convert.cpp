/*
 * Convert.cpp
 *
 *  Created on: 15.10.2012
 *      Author: christian
 */

#include "Convert.hpp"

namespace MathConvert
{

	Convert::Convert()
	{
		// TODO Auto-generated constructor stub

	}

	Convert::~Convert()
	{
		// TODO Auto-generated destructor stub
	}

	double Convert::deg2rad(double angle)
	{
		return(angle * 3.14159265358979323846 / 180.0);
	}

	double Convert::rad2deg(double angle)
	{
		return(angle * 180.0 / 3.14159265358979323846);
	}

} /* namespace MathConvert */
