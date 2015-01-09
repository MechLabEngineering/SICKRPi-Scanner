/**
 * @file BufferException.hpp
 *
 * Enthält Klasse BufferException, die allgemeine Exceptionklasse für die verschiedenen Bufferarten, die durch die STL oder z.B. Boost nicht abgebildet werden.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef BUFFEREXCEPTION_HPP_
#define BUFFEREXCEPTION_HPP_

#include <exception>

/** @brief verschiedene Pufferarten.
 *
 * Der Namespace buffer kapselt alle Arten von verschiedenen Pufferarten, die durch die STL oder z.B. Boost nicht abgebildet werden.
 */
namespace buffer
{
	/** @brief allgemeine Exception.
	 *
	 * Die allgemeine Exceptionklasse für verschiedene Pufferarten.
	 */
	class BufferException : public std::exception
	{
	public:
		BufferException(){};
		virtual ~BufferException() throw(){};
		virtual const char* what() const throw()
		{
			return("In der Pufferklasse trat ein unbekannter Fehler auf.");
		}
	};

} /* namespace buffer */
#endif /* BUFFEREXCEPTION_HPP_ */
