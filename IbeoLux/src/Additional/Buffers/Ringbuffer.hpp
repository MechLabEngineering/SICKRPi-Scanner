/**
 * @file Ringbuffer.hpp
 *
 * Enthält Klasse Ringbuffer, die einen Ringbuffer auf Byte-Basis bereitstellt.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef RINGBUFFER_HPP_
#define RINGBUFFER_HPP_

#include "RingbufferException.hpp"

namespace buffer
{
	/**
	 * Flags die angeben, ob ein Überschreiben der Daten in dem Ringpuffer erlaubt ist oder nicht.
	 */
	enum BUFFEROVERFLOW { ALLOWED, DENIED };

	/** @brief Ringpuffer für Bytedaten.
	 *
	 * Die Klasse stellt einen Ringpuffer bereit, der Bytes beliebiger Länge speichern kann.
	 */
	class Ringbuffer
	{
	public:
		Ringbuffer(unsigned int bufferSize, BUFFEROVERFLOW overflow);
		virtual ~Ringbuffer() throw();

		void setSize(unsigned int bufferSize);
		void readBytes(unsigned char **readPtrOne, unsigned char **readPtrTwo, unsigned int *readBytesOne, unsigned int *readBytesTwo, unsigned int bytes, bool concurrentDeleting);
		void writeBytes(unsigned char* byteSequence, unsigned int bytes);
		void copyBytes(unsigned char* byteBuffer, unsigned int bytes);
		const unsigned int& getReadBytesLeft() const;
		const unsigned int& getBufferSize() const;

	private:
		const BUFFEROVERFLOW overflow;
		unsigned int bufferSize;
		unsigned char *bufferBegin, *bufferEnd;
		unsigned char *writePtr;
		unsigned char *readPtrOne, *readPtrTwo, *readNext;
		unsigned int readBytesOne, readBytesTwo;
		unsigned int readBytesLeft;
	};

} /* namespace buffer */
#endif /* RINGBUFFER_HPP_ */
