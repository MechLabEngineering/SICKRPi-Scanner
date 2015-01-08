/**
 * @file RingbufferException.hpp
 *
 * Enthält die Exceptionklassen für den Ringpuffer.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef RINGBUFFEREXCEPTION_HPP_
#define RINGBUFFEREXCEPTION_HPP_

#include "BufferException.hpp"

namespace buffer
{
	/** @brief allgemeine Ringpuffer-Exceptionklasse
	 *
	 * Die allgemeine Ringpuffer-Exceptionklasse
	 */
	class RingbufferException : public buffer::BufferException
	{
	public:
		RingbufferException(){};
		virtual ~RingbufferException() throw(){};
		virtual const char* what() const throw()
		{
			return("In der Ringpufferklasse trat ein unbekannter Fehler auf.");
		}
	};


	/** @brief mehr Bytes gelesen, als vorhanden
	 *
	 * Die Exception wird immer dann geworfen, wenn versucht wird mehr Bytes zu lesen, als im Ringpuffer vorhanden sind.
	 */
	class RingbufferReadOverflowException : public RingbufferException
	{
	public:
		RingbufferReadOverflowException(){};

		virtual ~RingbufferReadOverflowException() throw(){};

		virtual const char* what() const throw()
		{
			return("Es koennen nicht mehr Bytes gelesen werden, als der Ringpuffer gross ist oder dieser an ungelesenen Daten enthaelt.");
		}
	};

	/** @brief Puffer kleiner, als Menge an zu schreibenden Daten
	 *
	 * Die Exception wird immer dann geworfen, wenn versucht wird mehr Bytes in den Ringpuffer zu schreiben, als es die Größe zulässt. Diese Exception wird nur geworfen, wenn das Flag BUFFEROVERFLOW im Konstruktor auf DENIED gesetzt wurde.
	 */
	class RingbufferWriteOverflowException : public RingbufferException
	{
	public:

		RingbufferWriteOverflowException(){};

		virtual ~RingbufferWriteOverflowException() throw(){};

		virtual const char* what() const throw()
		{
			return("Es koennen nicht mehr Bytes auf einmal geschrieben werden, als der Ringpuffer gross ist.");
		}
	};

} /* namespace buffer */


#endif /* RINGBUFFEREXCEPTION_HPP_ */
