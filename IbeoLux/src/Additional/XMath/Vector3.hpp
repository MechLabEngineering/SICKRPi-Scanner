#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include "Vector.hpp"

namespace vectorCalc { class MovingMatrix; }

namespace vectorCalc
{

    enum dimension {X = 0, Y = 1, Z = 2};

    class Vector3 : public Vector
    {
        public:
            Vector3();
            Vector3(double x, double y, double z);
            virtual ~Vector3();
            Vector3(const Vector3& vectorToCopy);

            Vector3 crossProduct(const Vector3& rightSideVector);
            void transformWithMovingMatrix(const MovingMatrix& matrix);

            Vector3& operator= (const Vector3& rightSideVector);
            Vector3& operator+= (const Vector3& rightSideVector);
            Vector3& operator-= (const Vector3& rightSideVector);
            Vector3& operator*= (double value);
            Vector3 operator- () const ;

        protected:
        private:
    };

    Vector3 operator+ (const Vector3& leftSideVector, const Vector3& rightSideVector);
    Vector3 operator- (const Vector3& leftSideVector, const Vector3& rightSideVector);
    Vector3 operator* (double value, const Vector3& rightSideVector);
    Vector3 operator* (const Vector3& leftSideVector, double value);

} /* namespace vectorCalc */

#endif // VECTOR3_HPP
