#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <string>
#include <sstream>


namespace vectorCalc
{
    class Vector
    {
    public:
        Vector(unsigned int dimension);
        virtual ~Vector();
        Vector(const Vector& vectorToCopy);

        unsigned int getDimension();
        double getLength();
        void setToNullVector();
        void normalize();
        void negate();
        double getAngle(const Vector& rightSideVector);
        double dotProduct(const Vector& rightSideVector);

        double& operator[](unsigned int row);

        virtual Vector& operator= (const Vector& rightSideVector);
        virtual Vector& operator+= (const Vector& rightSideVector);
        virtual Vector& operator-= (const Vector& rightSideVector);
        virtual Vector& operator*= (double value);
        Vector operator- () const;

        std::string& toString();

    protected:
        double *vector;
        unsigned int dimension;
        std::string vectorAsString;
        std::stringstream sstream;
    };

    Vector operator+ (const Vector& leftSideVector, const Vector& rightSideVector);
    Vector operator- (const Vector& leftSideVector, const Vector& rightSideVector);
    Vector operator* (double value, const Vector& rightSideVector);
    Vector operator* (const Vector& leftSideVector, double value);

} /* namespace vectorCalc */

#endif // VECTOR_HPP
