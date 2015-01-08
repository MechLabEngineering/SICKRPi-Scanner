/*
 * MovingMatrix.hpp
 *
 *  Created on: 12.10.2012
 *      Author: christian
 */

#ifndef MOVINGMATRIX_HPP_
#define MOVINGMATRIX_HPP_

#include "Matrix.hpp"

namespace vectorCalc { class Vector3; }

namespace vectorCalc
{

	class MovingMatrix : public Matrix
	{
	public:
		MovingMatrix(bool setToIdentity = true);
		virtual ~MovingMatrix();

		MovingMatrix& translate(double X, double Y, double Z);
		MovingMatrix& translate(Vector3& vector);
		MovingMatrix& rotateX(double angle); // im Radiantenmaß
		MovingMatrix& rotateY(double angle); // im Radiantenmaß
		MovingMatrix& rotateZ(double angle); // im Radiantenmaß
		MovingMatrix& scale(double scalingFactor);

		MovingMatrix& operator=  (const MovingMatrix& rightSideMatrix);
		MovingMatrix& operator+= (const MovingMatrix& rightSideMatrix);
		MovingMatrix& operator-= (const MovingMatrix& rightSideMatrix);
		MovingMatrix& operator*= (const MovingMatrix& rightSideMatrix);
		MovingMatrix& operator*= (const double);
		MovingMatrix operator-() const;

	};

	MovingMatrix operator+ (const MovingMatrix& leftSideMatrix, const MovingMatrix& rightSideMatrix);
	MovingMatrix operator- (const MovingMatrix& leftSideMatrix, const MovingMatrix& rightSideMatrix);
	MovingMatrix operator* (const MovingMatrix& leftSideMatrix, const MovingMatrix& rightSideMatrix);
	MovingMatrix operator* (double value, const MovingMatrix& rightSideMatrix);
	MovingMatrix operator* (const MovingMatrix& leftSideMatrix, double value);
	Vector3 operator* (const MovingMatrix& leftSideMatrix, const Vector3& vector);

} /* namespace vectorCalc */
#endif /* MOVINGMATRIX_HPP_ */
