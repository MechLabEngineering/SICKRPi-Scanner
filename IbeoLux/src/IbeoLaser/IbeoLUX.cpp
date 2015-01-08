/**
 * @file IbeoLUX.hpp
 *
 * Enthält Klasse IbeoLUX, die Verbindung zu einem ibeoLUX-Laserscanner aufnehmen kann und die individuellen Daten abruft und verarbeitet.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#include "IbeoLaserAbstract.hpp"
#include "IbeoLUX.hpp"
#include "IbeoLUXData.hpp"

#include <iostream>
#include <list>
#include <cstring>

#include <boost/asio/detail/socket_ops.hpp>

#include "../Additional/Buffers/RingbufferException.hpp"
#include "../Additional/Timestamp/Timestamp.hpp"
#include "../Additional/Convert/Convert.hpp"
#include "../Additional/XMath/MovingMatrix.hpp"
#include "../Additional/XMath/Convert.hpp"


#include "../Capture/ProcessAndCaptureAbstract.hpp"

namespace ibeo
{

	namespace ibeoLUX
	{
		IbeoLUX *IbeoLUX::singletonInstance = 0;

		IbeoLUX *IbeoLUX::getInstance()
		{
			return(singletonInstance);
		}

		void IbeoLUX::initInstance(const std::string& IP_address, unsigned short port, unsigned int history)
		{
			if(singletonInstance) singletonInstance->releaseInstance();

			singletonInstance = new IbeoLUX(IP_address, port, history);
		}

		void IbeoLUX::releaseInstance()
		{
			if(singletonInstance)
			{
				singletonInstance->stopMeasuring();
				singletonInstance->disconnect();
				delete singletonInstance;
				singletonInstance = 0;
			}
		}


		/**
		 * Der Konstruktor der Klasse.
		 *
		 * @param[in] IP_address Die IP-Adresse, zu der die Verbindung mit dem Laserscanner aufgebaut werden soll.
		 * @param[in] port       Der Port über den die Verbindung laufen soll.
		 * @param[in] history    Die Größe der Historie. Von dort können die neuesten und auch ältere Daten abgerufen werden.
		 */
		IbeoLUX::IbeoLUX(const std::string& IP_address,
						 unsigned short port,
						 unsigned int history
						)

						:

						IbeoLaserAbstract(IP_address, port, 0xAFFEC0C2, 24, history)
		{
			float angle = -1.2;
			for(unsigned int i = 0; i < 4; i++, angle += 0.8)
			{
				zValueLayerAdjustmentMatrix[i].rotateY(MathConvert::Convert::deg2rad(angle));
			}

			fillWithEmptyIbeoLaserData();
		}

		/**
		 * Der Destruktor der Klasse.
		 */
		IbeoLUX::~IbeoLUX()
		{

		}

		/**
		 * füllt die deques mit NULL-Daten (Leerdaten), so dass später einige IF-Konstrukte gespart werden können beim Abruf von DataObjekten.
		 */
		void IbeoLUX::fillWithEmptyIbeoLaserData()
		{
			for(unsigned int i = 0; i < history; i++)
			{
				ibeoLaserDataAbstractSmartPtr dat(new IbeoLUXData());
				data.push_front(dat);
			}
		}

		/**
		 * verarbeitet den Header einer Messdatenreihe
		 */
		bool IbeoLUX::processMessageHeader(bool allowSynchronisation)
		{
			while(ringBuffer->getReadBytesLeft() < headerSize)
				this->receiveData(TMPBUFSIZE);

			unsigned char *readPtrOne, *readPtrTwo;
			unsigned int readBytesOne, readBytesTwo;

			unsigned int offset = 0;

			try
			{
				ringBuffer->readBytes(&readPtrOne, &readPtrTwo, &readBytesOne, &readBytesTwo, headerSize, true);
			}
			catch(buffer::RingbufferReadOverflowException&)
			{
				std::cerr << "Es wurde versucht mehr Daten zu lesen, als im Ringpuffer enthalten sind." << "[HEADERSIZE=" << headerSize << "]"<< std::endl;
			}
			if(readBytesOne) memcpy(tmpBuffer, readPtrOne, readBytesOne);
			if(readBytesTwo) memcpy(tmpBuffer + readBytesOne, readPtrTwo, readBytesTwo);

			// Daten liegen in Netzwork-Byteorder vor; müssen also vor der Verarbeitung für x86-CPU umgewandelt werden

			namespace boost_socket_ops = boost::asio::detail::socket_ops; // der namespace macht Network-Byte-Order-Operationen plattformunabhängig verfügbar

			// Magicword einlesen: Offset 0/0x00, Länge 4
			memcpy(&(magicWord), tmpBuffer, 4);
			magicWord = boost_socket_ops::network_to_host_long(magicWord);
			if(magicWord != MAGICWORD)
			{
				if(allowSynchronisation) synchronizeToStream();
				return(false);
			}
			//else std::cout << std::hex << magicWord << std::endl;

			// BodySize einlesen: Offset 8/0x08, Länge 4
			offset += 8;
			memcpy(&(bodySize), tmpBuffer + offset, 4);
			bodySize = boost_socket_ops::network_to_host_long(bodySize);

			// DataType einlesen: Offset 14/0x0E, Länge 2
			offset += 6;
			memcpy(&(dataType), tmpBuffer + offset, 2);
			dataType = static_cast<ibeo::DATATYPE>(boost_socket_ops::network_to_host_short(dataType));

			// da es verschiedene Arten von Paketen gibt, müssen die relevaten SCANDATA-Pakete herausgefiltert werden
			// die Daten die einem NICHT-SCANDATA-Paket anhängen, müssen zwar abgerufen werden aus dem Ringbuffer, aber sie werden nicht verarbeitet.
			if(dataType != IbeoLUX_SCANDATA)
			{
				while(ringBuffer->getReadBytesLeft() < bodySize)
					this->receiveData(TMPBUFSIZE);

				try
				{
					ringBuffer->readBytes(&readPtrOne, &readPtrTwo, &readBytesOne, &readBytesTwo, bodySize, true);
				}
				catch(buffer::RingbufferReadOverflowException&)
				{
					std::cerr << "Es wurde versucht mehr Daten zu lesen, als im Ringpuffer enthalten sind." << "[bodySize=" << bodySize << ", buffer= " << ringBuffer->getReadBytesLeft() << ", SCANDATA=NO]"<< std::endl;
				}
				return(false);
			}

			/* ********************************************************************************************************************************************
			 *  Timestamp des Lasers nur dann einlesen, wenn es erforderlich wird. Jetzt wird Timestamp global und statisch über Timestamp-klasse abgerufen
			 * ********************************************************************************************************************************************
			// TimeStamp einlesen: Offset 18/0x12, Länge 4
			offset += 2;
			memcpy(&(timeStampSeconds), tmpBuffer + offset, 4);
			timeStampSeconds = boost_socket_ops::network_to_host_long(timeStampSeconds);

			// TimeStamp einlesen: Offset 22/0x16, Länge 4
			offset += 4;
			memcpy(&(timeStampMilliseconds), tmpBuffer + offset, 4);
			timeStampMilliseconds = boost_socket_ops::network_to_host_long(timeStampMilliseconds);
			*/
			return(true);
		}

		/**
		 * verarbeitet die nachgelagerten Daten nach dem Header, jedoch nicht die Messdaten selbst, und legt ein passendes DataObjekt an.
		 */
		void IbeoLUX::processScanDataHeader()
		{
			while(ringBuffer->getReadBytesLeft() < bodySize)
			{
				this->receiveData(TMPBUFSIZE);
			}

			unsigned char *readPtrOne, *readPtrTwo;
			unsigned int readBytesOne, readBytesTwo;

			try
			{
				ringBuffer->readBytes(&readPtrOne, &readPtrTwo, &readBytesOne, &readBytesTwo, bodySize, true);
			}
			catch(buffer::RingbufferReadOverflowException&)
			{
				std::cerr << "Es wurde versucht mehr Daten zu lesen, als im Ringpuffer enthalten sind. [bodySize=" << bodySize << ", SCANDATA=YES]" << std::endl;
			}
			if(readBytesOne) memcpy(tmpBuffer, readPtrOne, readBytesOne);
			if(readBytesTwo) memcpy(tmpBuffer + readBytesOne, readPtrTwo, readBytesTwo);

			

			unsigned char* dataPointer = tmpBuffer;

			// ScanNummer einlesen: Offset 0/0x00, Länge 2
			memcpy(&(scanNumber), dataPointer, 2);

			// Angleticks per Rotation einlesen: Offset 22/0x16, Länge 2
			memcpy(&(angleTicksPerRotation), (dataPointer + 22), 2);

			// Anzahl an Scanpunkten einlesen: Offset 28/0x1C, Länge 2
			memcpy(&(numberOfScanPoints), (dataPointer + 28), 2);

			// Pointer versetzen; vorher: Scandaten inkl. Headerinformationen, nachher: Scanpunktliste: Offset 44/0x2C, Länge <numberOfScanPoints>
			dataPointer = (dataPointer + 44);

			ibeoLaserDataAbstractSmartPtr dat(new IbeoLUXData(bodySize, scanNumber, numberOfScanPoints, angleTicksPerRotation, dataPointer));
			if(disclosureMethod) disclosureMethod(dat);
			appendData(dat);
			//std::cout << "SNR:" << scanNumber << std::endl;
			static int i = 0;
			
			if (i == 0) {
				//imuData();
				ibeoLaserDataAvailable(dat);
				i = 0;
			} else
				i++;

		}



	} // namespace ibeoLUX
} /* namespace ibeo */
