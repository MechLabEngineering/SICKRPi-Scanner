#ifndef VECTOREXCEPTION_HPP
#define VECTOREXCEPTION_HPP

namespace vectorCalc
{
	class VectorException : public std::exception
	{
	public:
		VectorException() {}
		virtual ~VectorException() throw() {}

		virtual const char* what() const throw()
		{
			return("Es ist ein unbekannter Fehler bei der Operation der Matrix aufgetreten.");
		}
	};

	class VectorDimensionsIncorrectException : public VectorException
	{
	public:
		VectorDimensionsIncorrectException() {}
		~VectorDimensionsIncorrectException() throw() {}

		virtual const char* what() const throw()
		{
			return("Die Dimensionen des Vektors stimmen nicht überein. Die Operation ist deshalb nicht ausführbar.");
		}
	};
}

#endif // VECTOREXCEPTION_HPP
