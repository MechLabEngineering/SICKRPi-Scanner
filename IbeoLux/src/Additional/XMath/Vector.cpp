#include "Vector.hpp"
#include "VectorException.hpp"
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

namespace vectorCalc
{
    Vector operator+ (const Vector& leftSideVector, const Vector& rightSideVector)
    {
        Vector tmpVec(leftSideVector);
        tmpVec += rightSideVector;
        return(tmpVec);
    }

    Vector operator- (const Vector& leftSideVector, const Vector& rightSideVector)
    {
        Vector tmpVec(leftSideVector);
        tmpVec -= rightSideVector;
        return(tmpVec);
    }

    Vector operator* (double value, const Vector& rightSideVector)
    {
        Vector tmpVec(rightSideVector);
        tmpVec *= value;
        return(tmpVec);
    }

    Vector operator* (const Vector& leftSideVector, double value)
    {
        Vector tmpVec(leftSideVector);
        tmpVec *= value;
        return(tmpVec);
    }



    Vector::Vector(unsigned int dimension) :
        dimension(dimension)
    {
        vector = new double[dimension];
        setToNullVector();
    }

    Vector::~Vector()
    {
        delete[] vector;
    }

    Vector::Vector(const Vector& vectorToCopy) :
        dimension(vectorToCopy.dimension)
    {
        vector = new double[dimension];
        for(unsigned int i = 0; i < dimension; i++)
            vector[i] = vectorToCopy.vector[i];
    }



    double& Vector::operator[](unsigned int row)
	{
		return vector[row];
	}


    Vector& Vector::operator= (const Vector& rightSideVector)
    {
        this->dimension = rightSideVector.dimension;

        double* tmpVec = new double[dimension];
        for(unsigned int i = 0; i < dimension; i++)
            tmpVec[i] = rightSideVector.vector[i];

        delete[] vector;
        vector = tmpVec;

        return(*this);
    }

    Vector& Vector::operator+= (const Vector& rightSideVector)
    {
        if(dimension != rightSideVector.dimension) throw(VectorDimensionsIncorrectException());

        for(unsigned int i = 0; i < dimension; i++)
        {
            vector[i] += rightSideVector.vector[i];
        }

        return(*this);
    }


    Vector& Vector::operator-= (const Vector& rightSideVector)
    {
        if(dimension != rightSideVector.dimension) throw(VectorDimensionsIncorrectException());

        for(unsigned int i = 0; i < dimension; i++)
        {
            vector[i] -= rightSideVector.vector[i];
        }

        return(*this);
    }

    Vector& Vector::operator*= (double value)
    {
        for(unsigned int i = 0; i < dimension; i++)
        {
            vector[i] *= value;
        }

        return(*this);
    }


    Vector Vector::operator- () const
    {
        Vector tmpVec(*this);

        for(unsigned int i = 0; i < dimension; i++)
            tmpVec.vector[i] = vector[i] *= 1.0;

        return(*this);
    }


    unsigned int Vector::getDimension()
    {
        return(dimension);
    }

    double Vector::getLength()
    {
        double value = 0.0;

        for(unsigned int i = 0; i < dimension; i++)
            value += vector[i] * vector[i];

        return(std::sqrt(value));
    }

    void Vector::setToNullVector()
    {
        for(unsigned int i = 0; i < dimension; i++)
            vector[i] = 0.0;
    }

    void Vector::normalize()
    {
        double length = this->getLength();

        if(length != 0.0)
        for(unsigned int i = 0; i < dimension; i++)
            vector[i] /= length;
    }

    void Vector::negate()
    {
        for(unsigned int i = 0; i < dimension; i++)
            vector[i] *= -1.0;
    }

    double Vector::dotProduct(const Vector& rightSideVector)
    {
        double value = 0.0;

        for(unsigned int i = 0; i < dimension; i++)
            value += vector[i] * rightSideVector.vector[i];

        return(value);
    }


    std::string& Vector::toString()
    {
        unsigned int countHyphen = 14;
		sstream.str("");

		sstream << "---[" << dimension << "]";
		int currentLength = sstream.str().length();

		for(unsigned int i = 0; i < (countHyphen - currentLength); i++)
			sstream << "-";


		sstream << std::endl;

		for(unsigned int dimRows = 0; dimRows < dimension; dimRows++)
		{
			sstream << "| " << std::setprecision(4) << std::scientific << vector[dimRows] << " |" << std::endl;
			for(unsigned int i = 0; i < countHyphen; i++)
				sstream << "-";
			sstream << std::endl;
		}

		vectorAsString = sstream.str();
		return(vectorAsString);
    }

} /* namespace vectorCalc */
