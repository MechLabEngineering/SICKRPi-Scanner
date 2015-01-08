/*
 * Matrix.hpp
 *
 *  Created on: 09.10.2012
 *      Author: christian
 */

#ifndef MATRIX_HPP_
#define MATRIX_HPP_


#include <vector>
#include <string>
#include <sstream>

#include "MatrixException.hpp"

namespace vectorCalc
{
    class Matrix
    {
    public:
        Matrix(unsigned int dimensionRows, unsigned int dimensionCols, bool setToNullMatrix = true);
        Matrix(const Matrix& matrixToCopy);
        virtual ~Matrix();

        unsigned int getDimensionCols();
        unsigned int getDimensionRows();
        void setToIndentity();
        void setToNullMatrix();
        void transpose();
        bool isQuadric();
        void negate();
        double* getMatrixVector();
        std::string& toString();

        double& operator() (unsigned int row, unsigned int column);


        Matrix& operator=  (const Matrix& rightSideMatrix);
        Matrix& operator+= (const Matrix& rightSideMatrix);
        Matrix& operator-= (const Matrix& rightSideMatrix);
        Matrix& operator*= (const Matrix& rightSideMatrix);
        Matrix& operator*= (const double);
        Matrix operator-() const;

    protected:
        void clear();

        double* matrix;
        unsigned int dimensionCols, dimensionRows;
        std::string matrixAsString;
        std::stringstream sstream;

};

Matrix operator+ (const Matrix& leftSideMatrix, const Matrix& rightSideMatrix);
Matrix operator- (const Matrix& leftSideMatrix, const Matrix& rightSideMatrix);
Matrix operator* (const Matrix& leftSideMatrix, const Matrix& rightSideMatrix);
Matrix operator* (double value, const Matrix& rightSideMatrix);
Matrix operator* (const Matrix& leftSideMatrix, double value);



} /* namespace vectorCalc */

#endif /* MATRIX_HPP_ */
