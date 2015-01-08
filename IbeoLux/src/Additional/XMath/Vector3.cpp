#include "../Defines/Defines.hpp"
#include "Vector.hpp"
#include "Vector3.hpp"
#include "VectorException.hpp"
#include "MovingMatrix.hpp"

namespace vectorCalc
{

	Vector3 operator+ (const Vector3& leftSideVector, const Vector3& rightSideVector)
    {
        Vector3 tmpVec(leftSideVector);
        tmpVec += rightSideVector;
        return(tmpVec);
    }

	Vector3 operator- (const Vector3& leftSideVector, const Vector3& rightSideVector)
    {
        Vector3 tmpVec(leftSideVector);
        tmpVec -= rightSideVector;
        return(tmpVec);
    }

	Vector3 operator* (double value, const Vector3& rightSideVector)
    {
		Vector3 tmpVec(rightSideVector);
        tmpVec *= value;
        return(tmpVec);
    }

	Vector3 operator* (const Vector3& leftSideVector, double value)
    {
		Vector3 tmpVec(leftSideVector);
        tmpVec *= value;
        return(tmpVec);
    }


    Vector3::Vector3() :
        Vector(3)
    {

    }

    Vector3::Vector3(double x, double y, double z) :
    	Vector(3)
    {
    	vector[X] = x;
    	vector[Y] = y;
    	vector[Z] = z;
    }

    Vector3::~Vector3()
    {

    }


    Vector3::Vector3(const Vector3& vectorToCopy) :
		Vector(vectorToCopy.dimension)
    {
    	vector = new double[dimension];
		for(unsigned int i = 0; i < dimension; i++)
			vector[i] = vectorToCopy.vector[i];
    }

    Vector3 Vector3::crossProduct(const Vector3& rightSideVector)
    {
        Vector3 tmpVec;
        Vector3& nonStaticVec = const_cast<Vector3&>(rightSideVector);

        tmpVec[X] = (*this)[Y] * nonStaticVec[Z] - (*this)[Z] * nonStaticVec[Y];
        tmpVec[Y] = (*this)[Z] * nonStaticVec[X] - (*this)[X] * nonStaticVec[Z];
        tmpVec[Z] = (*this)[X] * nonStaticVec[Y] - (*this)[Y] * nonStaticVec[X];

        return(tmpVec);
    }

    void Vector3::transformWithMovingMatrix(const MovingMatrix& matrix)
    {
    	Matrix columnMat(4, 1);
    	columnMat(0, 0) = vector[X];
    	columnMat(1, 0) = vector[Y];
    	columnMat(2, 0) = vector[Z];
    	columnMat(3, 0) = 0; // homogene Koordinate

		Matrix transformedColumnMat = matrix * columnMat;

		double homogenCoord = transformedColumnMat(3, 0);

		if(homogenCoord > 0.0)
		{
			vector[X] = transformedColumnMat(0, 0) / transformedColumnMat(3, 0);
			vector[Y] = transformedColumnMat(1, 0) / transformedColumnMat(3, 0);
			vector[Z] = transformedColumnMat(2, 0) / transformedColumnMat(3, 0);
		}
		else
		{
			vector[X] = transformedColumnMat(0, 0);
			vector[Y] = transformedColumnMat(1, 0);
			vector[Z] = transformedColumnMat(2, 0);
		}
    }

    Vector3& Vector3::operator= (const Vector3& rightSideVector)
	{
		this->dimension = rightSideVector.dimension;

		double* tmpVec = new double[dimension];
		for(unsigned int i = 0; i < dimension; i++)
			tmpVec[i] = rightSideVector.vector[i];

		delete[] vector;
		vector = tmpVec;

		return(*this);
	}

    Vector3& Vector3::operator+= (const Vector3& rightSideVector)
	{
		if(dimension != rightSideVector.dimension) throw(VectorDimensionsIncorrectException());

		for(unsigned int i = 0; i < dimension; i++)
		{
			vector[i] += rightSideVector.vector[i];
		}

		return(*this);
	}


    Vector3& Vector3::operator-= (const Vector3& rightSideVector)
	{
                if(dimension != rightSideVector.dimension) throw(VectorDimensionsIncorrectException());

		for(unsigned int i = 0; i < dimension; i++)
		{
			vector[i] -= rightSideVector.vector[i];
		}

		return(*this);
	}

    Vector3& Vector3::operator*= (double value)
	{
		for(unsigned int i = 0; i < dimension; i++)
		{
			vector[i] *= value;
		}

		return(*this);
	}


    Vector3 Vector3::operator- () const
	{
    	Vector3 tmpVec(*this);

		for(unsigned int i = 0; i < dimension; i++)
			tmpVec.vector[i] = vector[i] *= 1.0;

		return(*this);
	}

} /* namespace vectorCalc */
