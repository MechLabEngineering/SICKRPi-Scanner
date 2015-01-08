/*
 * Convert.hpp
 *
 *  Created on: 15.10.2012
 *      Author: christian
 */

#ifndef CONVERTXMATH_HPP_
#define CONVERTXMATH_HPP_

namespace MathConvert
{

class Convert
{
public:
	Convert();
	virtual ~Convert();

	static double deg2rad(double angle);
	static double rad2deg(double angle);
};

} /* namespace MathConvert */
#endif /* CONVERTXMATH_HPP_ */
