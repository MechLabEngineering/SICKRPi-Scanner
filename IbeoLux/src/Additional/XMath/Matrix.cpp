/*
 * Matrix.cpp
 *
 *  Created on: 11.10.2012
 *      Author: christian
 */



#include "Matrix.hpp"
#include "MatrixException.hpp"
#include <iomanip>

namespace vectorCalc
{
    Matrix operator+ (const Matrix& leftSideMatrix, const Matrix& rightSideMatrix)
	{
		Matrix tmpMat(leftSideMatrix);
		tmpMat += rightSideMatrix;
		return(tmpMat);
	}

	Matrix operator- (const Matrix& leftSideMatrix, const Matrix& rightSideMatrix)
	{
		Matrix tmpMat(leftSideMatrix);
		tmpMat -= rightSideMatrix;
		return(tmpMat);
	}

    Matrix operator* (const Matrix& leftSideMatrix, const Matrix& rightSideMatrix)
	{
		Matrix tmpMat(leftSideMatrix);
		tmpMat *= rightSideMatrix;
		return(tmpMat);
	}

	Matrix operator* (double value, const Matrix& rightSideMatrix)
	{
		Matrix tmpMat(rightSideMatrix);
		tmpMat *= value;
		return(tmpMat);
	}

	Matrix operator* (const Matrix& leftSideMatrix, double value)
	{
		Matrix tmpMat(leftSideMatrix);
		tmpMat *= value;
		return(tmpMat);
	}

	double& Matrix::operator()(unsigned int row, unsigned int column)
	{
		return(matrix[row * dimensionCols + column]);
	}

	Matrix& Matrix::operator= (const Matrix& rightSideMatrix)
	{
		Matrix& mat = const_cast<Matrix&>(rightSideMatrix);

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




	Matrix& Matrix::operator+= (const Matrix& rightSideMatrix)
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


	Matrix& Matrix::operator-= (const Matrix& rightSideMatrix)
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


	Matrix Matrix::operator-() const
	{
		Matrix tmpMat(*this);
		tmpMat.negate();
		return(tmpMat);
	}



	Matrix& Matrix::operator*= (const Matrix& rightSideMatrix)
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


	Matrix& Matrix::operator*= (const double value)
	{
		for(unsigned int row = 0; row < this->dimensionRows; row++)
			for(unsigned int col = 0; col < this->dimensionCols; col++)
			{
				matrix[row * dimensionCols + col] *= value;
			}

		return(*this);
	}


	Matrix::Matrix(unsigned int dimensionRows, unsigned int dimensionCols, bool setToNullMatrix) :
			dimensionCols(dimensionCols),
			dimensionRows(dimensionRows)
	{
		matrix = new double[dimensionRows * dimensionCols];
		if(setToNullMatrix) this->setToNullMatrix();
	}


	Matrix::Matrix(const Matrix& matrixToCopy) :
		dimensionCols(matrixToCopy.dimensionCols),
		dimensionRows(matrixToCopy.dimensionRows)
	{
		Matrix& mat = const_cast<Matrix&>(matrixToCopy);

		matrix = new double[dimensionRows * dimensionCols];

		for(unsigned int row = 0; row < dimensionRows; row++)
			for(unsigned int col = 0; col < dimensionCols; col++)
			{
				matrix[row * dimensionCols + col] = mat.matrix[row * dimensionCols + col];
			}
	}



	Matrix::~Matrix()
	{
		clear();
	}


	void Matrix::clear()
	{
		delete[] matrix;
	}


	unsigned int Matrix::getDimensionCols()
	{
		return(dimensionCols);
	}


	unsigned int Matrix::getDimensionRows()
	{
		return(dimensionRows);
	}

	void Matrix::setToIndentity()
	{
		if(!isQuadric()) throw(MatrixNotQuadraticException()); // Einheitsmatrix ist nur für quadratische Matrizen definiert

		setToNullMatrix(); // Matrix erst einmal löschen

		for(unsigned int i = 0; i < dimensionCols; i++)
		{
			matrix[i * dimensionCols + i] = 1;
		}
	}


	void Matrix::setToNullMatrix()
	{
		for(unsigned int cols = 0; cols < dimensionCols; cols++)
			for(unsigned int rows = 0; rows < dimensionRows; rows++ )
			{
				matrix[rows * dimensionCols + cols] = 0;
			}
	}


	bool Matrix::isQuadric()
	{
		return(dimensionCols == dimensionRows);
	}


	void Matrix::negate()
	{
		for(unsigned int cols = 0; cols < dimensionCols; cols++)
			for(unsigned int rows = 0; rows < dimensionRows; rows++ )
			{
				matrix[rows * dimensionCols + cols] *= -1.0;
			}
	}

    double* Matrix::getMatrixVector()
    {
    	return(matrix);
    }


	void Matrix::transpose()
	{
		double* tmpMatrix = new double[dimensionCols * dimensionRows];

		for(unsigned int row = 0; row < dimensionCols; row++)
			for(unsigned int col = 0; col < dimensionRows; col++)
			{
				tmpMatrix[row * dimensionRows + col] = matrix[col * dimensionCols + row];
			}
		clear();

		matrix = tmpMatrix;
		unsigned int tmp = dimensionCols;
		dimensionCols = dimensionRows;
		dimensionRows = tmp;
	}



	std::string& Matrix::toString()
	{
		unsigned int countHyphen = dimensionCols * 13 + 1;
		sstream.str("");

		sstream << "---[" << dimensionRows << ", " << dimensionCols << "]";
		int currentLength = sstream.str().length();

		for(unsigned int i = 0; i < (countHyphen - currentLength); i++)
			sstream << "-";


		sstream << std::endl;

		for(unsigned int dimRows = 0; dimRows < dimensionRows; dimRows++)
		{
			sstream << "| ";

			for(unsigned int dimCols = 0; dimCols < dimensionCols; dimCols++)
			{
				sstream << std::setprecision(4) << std::scientific << matrix[dimRows * dimensionCols + dimCols] << " | ";
			}
			sstream << std::endl;
			for(unsigned int i = 0; i < countHyphen; i++)
				sstream << "-";
			sstream << std::endl;
		}

		matrixAsString = sstream.str();
		return(matrixAsString);
	}

} /* namespace vectorCalc */
