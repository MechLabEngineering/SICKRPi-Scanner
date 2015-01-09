/*
 * MovingMatrix.cpp
 *
 *  Created on: 12.10.2012
 *      Author: christian
 */

#include "MovingMatrix.hpp"
#include "Matrix.hpp"
#include "MatrixException.hpp"
#include <cmath>
#include "Vector3.hpp"

namespace vectorCalc
{

	MovingMatrix operator+ (const MovingMatrix& leftSideMatrix, const MovingMatrix& rightSideMatrix)
	{
	MovingMatrix tmpMat(leftSideMatrix);
		tmpMat += rightSideMatrix;
		return(tmpMat);
	}

	MovingMatrix operator- (const MovingMatrix& leftSideMatrix, const MovingMatrix& rightSideMatrix)
	{
		MovingMatrix tmpMat(leftSideMatrix);
		tmpMat -= rightSideMatrix;
		return(tmpMat);
	}

	MovingMatrix operator* (const MovingMatrix& leftSideMatrix, const MovingMatrix& rightSideMatrix)
	{
		MovingMatrix tmpMat(leftSideMatrix);
		tmpMat *= rightSideMatrix;
		return(tmpMat);
	}

    MovingMatrix operator* (double value, const MovingMatrix& rightSideMatrix)
	{
    	MovingMatrix tmpMat(rightSideMatrix);
		tmpMat *= value;
		return(tmpMat);
	}

	MovingMatrix operator* (const MovingMatrix& leftSideMatrix, double value)
	{
		MovingMatrix tmpMat(leftSideMatrix);
		tmpMat *= value;
		return(tmpMat);
	}

	Vector3 operator* (const MovingMatrix& leftSideMatrix, const Vector3& vector)
	{
		Vector3& tmpVec = const_cast<Vector3&>(vector);
		Matrix mat(4, 1);
		mat(0, 0) = tmpVec[X];
		mat(1, 0) = tmpVec[Y];
		mat(2, 0) = tmpVec[Z];
		mat(3, 0) = 0; // homogene Koordinate

		Matrix transformedColumnMat = leftSideMatrix * mat;

		Vector3 vec;
		if(transformedColumnMat(3, 0) != 0.0)
		{
			vec[X] = transformedColumnMat(0, 0) / transformedColumnMat(3, 0);
			vec[Y] = transformedColumnMat(1, 0) / transformedColumnMat(3, 0);
			vec[Z] = transformedColumnMat(2, 0) / transformedColumnMat(3, 0);
		}
		else
		{
			vec[X] = transformedColumnMat(0, 0);
			vec[Y] = transformedColumnMat(1, 0);
			vec[Z] = transformedColumnMat(2, 0);
		}

		return(vec);
	}

	MovingMatrix::MovingMatrix(bool setToIdentity) :
			Matrix(4, 4, false)
	{
		if(setToIdentity) setToIndentity();
	}

	MovingMatrix::~MovingMatrix()
	{

	}



	MovingMatrix& MovingMatrix::operator= (const MovingMatrix& rightSideMatrix)
	{
		MovingMatrix& mat = const_cast<MovingMatrix&>(rightSideMatrix);

		this->dimensionCols = rightSideMatrix.dimensionCols;
		this->dimensionRows = rightSideMatrix.dimensionRows;

		clear();
		matrix = new double[dimensionRows * dimensionCols];

		for(unsigned int row = 0; row < dimensionRows; row++)
			for(unsigned int col = 0; col < dimensionCols; col++)
			{
				matrix[row * dimensionCols + col] = mat.matrix[row * dimensionCols + col];
			}
		return(*this);
	}




	MovingMatrix& MovingMatrix::operator+= (const MovingMatrix& rightSideMatrix)
	{
		if(this->dimensionCols != rightSideMatrix.dimensionCols || this->dimensionRows != rightSideMatrix.dimensionRows)
			throw(MatrixDimensionsIncorrectException());

		for(unsigned int row = 0; row < this->dimensionRows; row++)
			for(unsigned int col = 0; col < this->dimensionCols; col++)
			{
				this->matrix[row * dimensionCols + col] += rightSideMatrix.matrix[row * dimensionCols + col];
			}

		return(*this);
	}


	MovingMatrix& MovingMatrix::operator-= (const MovingMatrix& rightSideMatrix)
	{
		if(this->dimensionCols != rightSideMatrix.dimensionCols || this->dimensionRows != rightSideMatrix.dimensionRows)
			throw(MatrixDimensionsIncorrectException());

		for(unsigned int row = 0; row < this->dimensionRows; row++)
			for(unsigned int col = 0; col < this->dimensionCols; col++)
			{
				this->matrix[row * dimensionCols + col] -= rightSideMatrix.matrix[row * dimensionCols + col];
			}

		return(*this);
	}


	MovingMatrix MovingMatrix::operator-() const
	{
		MovingMatrix tmpMat(*this);
		tmpMat.negate();
		return(tmpMat);
	}



	MovingMatrix& MovingMatrix::operator*= (const MovingMatrix& rightSideMatrix)
	{
		if(this->dimensionCols != rightSideMatrix.dimensionRows) throw(MatrixNotConcatenatedException());

		double* tmpMatrix = new double[this->dimensionRows * rightSideMatrix.dimensionCols];

		for(unsigned int row = 0; row < this->dimensionRows; row++)
			for(unsigned int col = 0; col < rightSideMatrix.dimensionCols; col++)
			{
				tmpMatrix[row * rightSideMatrix.dimensionCols + col] = 0;
				for(unsigned int counter = 0; counter < this->dimensionRows; counter++)
				{
					tmpMatrix[row * rightSideMatrix.dimensionCols + col]
						+= this->matrix[row * dimensionCols + counter]
						 * rightSideMatrix.matrix[counter * rightSideMatrix.dimensionCols + col];
				}
			}

		clear();
		this->matrix = tmpMatrix;
		this->dimensionCols = rightSideMatrix.dimensionCols;

		return(*this);
	}


	MovingMatrix& MovingMatrix::operator*= (const double value)
	{
		for(unsigned int row = 0; row < this->dimensionRows; row++)
			for(unsigned int col = 0; col < this->dimensionCols; col++)
			{
				matrix[row * dimensionCols + col] *= value;
			}

		return(*this);
	}






	MovingMatrix& MovingMatrix::translate(double X, double Y, double Z)
	{
		MovingMatrix transMat;

		transMat(0, 3) = X;
		transMat(1, 3) = Y;
		transMat(2, 3) = Z;

		*this *= transMat;

		return(*this);
	}

  	MovingMatrix& MovingMatrix::translate(Vector3& vector)
  	{
  		MovingMatrix transMat;

  		transMat(0, 3) = vector[X];
  		transMat(1, 3) = vector[Y];
  		transMat(2, 3) = vector[Z];

  		*this *= transMat;

		return(*this);
  	}


	MovingMatrix& MovingMatrix::rotateX(double angle)
	{
        MovingMatrix rotMat;
        double cosAngle = std::cos(angle);
        double sinAngel = std::sin(angle);
        rotMat(1, 1) = cosAngle;
        rotMat(2, 2) = cosAngle;
        rotMat(2, 1) = sinAngel;
        rotMat(1, 2) = -sinAngel;

        *this *= rotMat;

		return(*this);
	}

	MovingMatrix& MovingMatrix::rotateY(double angle)
	{
		MovingMatrix rotMat;
        double cosAngle = std::cos(angle);
        double sinAngel = std::sin(angle);
        rotMat(0, 0) = cosAngle;
        rotMat(2, 2) = cosAngle;
        rotMat(2, 0) = -sinAngel;
        rotMat(0, 2) = sinAngel;

        *this *= rotMat;

		return(*this);
	}

	MovingMatrix& MovingMatrix::rotateZ(double angle)
	{
		MovingMatrix rotMat;
        double cosAngle = std::cos(angle);
        double sinAngel = std::sin(angle);
        rotMat(0, 0) = cosAngle;
        rotMat(1, 1) = cosAngle;
        rotMat(1, 0) = sinAngel;
        rotMat(0, 1) = -sinAngel;

        *this *= rotMat;

		return(*this);
	}

	MovingMatrix& MovingMatrix::scale(double scalingFactor)
	{
	    MovingMatrix scaleMat;
	    scaleMat(3, 3) = 1.0 / scalingFactor;

	    *this *= scaleMat;

		return(*this);
	}


} /* namespace vectorCalc */
