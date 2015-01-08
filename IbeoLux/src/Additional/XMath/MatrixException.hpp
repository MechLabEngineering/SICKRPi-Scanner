/*
 * MatrixException.hpp
 *
 *  Created on: 10.10.2012
 *      Author: christian
 */

#ifndef MATRIXEXCEPTION_HPP_
#define MATRIXEXCEPTION_HPP_

#include <exception>


namespace vectorCalc
{
	class MatrixException : public std::exception
	{
	public:
		MatrixException() {}
		virtual ~MatrixException() throw() {}

		virtual const char* what() const throw()
		{
			return("Es ist ein unbekannter Fehler bei der Operation der Matrix aufgetreten.");
		}
	};

	class MatrixNotQuadraticException : public MatrixException
	{
	public:
		MatrixNotQuadraticException() {}
		~MatrixNotQuadraticException() throw() {}

		virtual const char* what() const throw()
		{
			return("Die Matrix ist nicht quadratisch und kann deshalb die Operation nicht ausführen.");
		}
	};

	class MatrixNotConcatenatedException : public MatrixException
	{
	public:
		MatrixNotConcatenatedException() {}
		~MatrixNotConcatenatedException() throw() {}

		virtual const char* what() const throw()
		{
			return("Die Matrizen sind nicht verkettet. Die Operation ist deshalb nicht ausführbar.");
		}
	};

	class MatrixDimensionsIncorrectException : public MatrixException
	{
	public:
		MatrixDimensionsIncorrectException() {}
		~MatrixDimensionsIncorrectException() throw() {}

		virtual const char* what() const throw()
		{
			return("Die Dimensionen der Matrizen stimmen nicht überein. Die Operation ist deshalb nicht ausführbar.");
		}
	};

} /* namespace vectorCalc */
#endif /* MATRIXEXCEPTION_HPP_ */
