/**
 * @file Ringbuffer.cpp
 *
 * Enthält die Implementierungen der Klasse Ringbuffer, die einen Ringbuffer auf Byte-Basis bereitstellt.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#include "Ringbuffer.hpp"

#include <cstring>
#include <iostream>

#include "RingbufferException.hpp"

namespace buffer
{
	/**
	 * Der Konstruktor der Klasse.
	 * @param[in] bufferSize Die Größe des Pufferspeichers.
	 * @param[in] overflow   Gibt an, ob ein Überschreiben noch ungelesener Daten ermöglicht werden soll.
	 */
	Ringbuffer::Ringbuffer(unsigned int bufferSize, BUFFEROVERFLOW overflow) :
			overflow(overflow), bufferSize(0), bufferBegin(0), bufferEnd(0), readBytesLeft(0)
	{
		setSize(bufferSize);

	}

	/**
	 * Der Destruktor der Klasse. Bei Aufruf werden alle darin gespeicherten Daten gelöscht.
	 */
	Ringbuffer::~Ringbuffer() throw()
	{
		delete[] this->bufferBegin;
	}

	/**
	 * Gibt eine neue Größe des Ringpuffers an. Bereits vorhandene Daten werden dabei gelöscht.
	 * @param[in] bufferSize Die Größe des Ringpuffers in Bytes.
	 */
	void Ringbuffer::setSize(unsigned int bufferSize)
	{
		this->bufferSize = bufferSize;
		if(this->bufferBegin != 0) delete[] this->bufferBegin;
		this->bufferBegin = new unsigned char[bufferSize];
		this->bufferEnd = bufferBegin + this->bufferSize;
		this->readPtrOne = this->readPtrTwo = 0;
		this->writePtr = this->readNext = this->bufferBegin;
		memset(bufferBegin, 0, bufferSize);
	}

	/**
	 * Ermöglicht das Lesen der Daten in dem Ringpuffer.
	 * Dazu werden die entsprechenden Pointer neu positioniert und in den jeweiligen Variablen die Anzahl der Bytes angegeben, die an den Pointern gelesen werden können.
	 *
	 * @param[out]	readPtrOne 			Der Pointer, der den ersten Teil der zu lesenden Daten repräsentiert.
	 * @param[out]	readPtrTwo 			Der Pointer, der den zweiten Teil der zu lesenden Daten repräsentiert.
	 * @param[out]	readBytesOne		Die Byteanzahl, die am ersten Pointer gelesen werden können.
	 * @param[out]	readBytesTwo		Die Byteanzahl, die am zweiten Pointer gelesen werden können.
	 * @param[in]	concurrentDeleting	Gibt an, ob die Daten für die Löschung freigegeben werden sollen oder nicht. Werden diese nicht freigegeben, werden sie beim erneuten Aufruf dieser Methode erneut gelesen.
	 */
	void Ringbuffer::readBytes(unsigned char **readPtrOne, unsigned char **readPtrTwo, unsigned int *readBytesOne, unsigned int *readBytesTwo, unsigned int bytes, bool concurrentDeleting)
	{
		if(overflow == buffer::DENIED)
		{
			if(bytes > bufferSize || bytes > readBytesLeft) throw(RingbufferReadOverflowException());
		}

		if((bufferEnd - readNext) >= bytes)
		{
			*readPtrTwo = 0;
			*readBytesTwo = 0;

			*readPtrOne = readNext;
			*readBytesOne = bytes;

			if(concurrentDeleting)
			{
				readNext += bytes;
				if(readNext >= bufferEnd) readNext -= bufferSize;
				readBytesLeft -= bytes;
			}
		}
		else
		{
			*readPtrOne = readNext;
			*readBytesOne = bufferEnd - readNext;

			*readPtrTwo = bufferBegin;
			*readBytesTwo = bytes - *readBytesOne;

			if(concurrentDeleting)
			{
				readNext = bufferBegin + *readBytesTwo;
				readBytesLeft -= bytes;
			}
		}
//		std::cout << "[RingBuf_readBytesLeft= " << readBytesLeft << "]" << std::endl;
	}

	/**
	 * Schreibt die angegebene Anzahl an Bytes in den Ringpuffer.
	 * @param[in] byteSequence 	Der Pointer auf einen Speicherbereich der kopiert werden soll.
	 * @oaram[in] byzes			Die Byteanzahl die kopiert werden soll.
	 */
	void Ringbuffer::writeBytes(unsigned char* byteSequence, unsigned int bytes)
	{
		if(overflow == buffer::DENIED)
		{
			if(bytes > bufferSize || bytes > (bufferSize - readBytesLeft)) throw(RingbufferWriteOverflowException());
		}

		unsigned int writeBytesTillPhysicalEnd = bufferEnd - writePtr;

		if(writeBytesTillPhysicalEnd < bytes) // wenn weniger Bytes an das Ende geschrieben werden können, als insg. geschrieben werden sollen
		{
			memcpy(writePtr, byteSequence, writeBytesTillPhysicalEnd);
			writePtr = bufferBegin;
			memcpy(writePtr, byteSequence + writeBytesTillPhysicalEnd, bytes - writeBytesTillPhysicalEnd);
			writePtr += bytes - writeBytesTillPhysicalEnd;
		}
		else
		{
			memcpy(writePtr, byteSequence, bytes);
			writePtr += bytes;
		}
		readBytesLeft += bytes;
	}

	void Ringbuffer::copyBytes(unsigned char* byteBuffer, unsigned int bytes)
	{
		if(overflow == buffer::DENIED)
		{
			if(bytes > bufferSize || bytes > readBytesLeft) throw(RingbufferReadOverflowException());
		}

		unsigned char *readPtrOne, *readPtrTwo;
		unsigned int readBytesOne, readBytesTwo;

		this->readBytes(&readPtrOne, &readPtrTwo, &readBytesOne, &readBytesTwo, bytes, false);

		if(readBytesOne) memcpy(byteBuffer, readPtrOne, readBytesOne);
		if(readBytesTwo) memcpy(byteBuffer + readBytesOne, readPtrTwo, readBytesTwo);
	}


	/**
	 * Gibt die Anzahl an Bytes zurück, die noch vom Ringpuffer gelesen werden können.
	 * @return Anzahl an lesbaren Bytes im Ringbuffer.
	 */
	const unsigned int& Ringbuffer::getReadBytesLeft() const
	{
		return(this->readBytesLeft);
	}

	/**
	 * Liefert die Größe des Ringpuffers zurück,
	 * @return Die Größe des Ringpuffers.
	 */
	const unsigned int& Ringbuffer::getBufferSize() const
	{
		return(this->bufferSize);
	}

} /* namespace buffer */
