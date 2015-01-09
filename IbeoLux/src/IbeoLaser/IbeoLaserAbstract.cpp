/**
 * @file IbeoLaserAbstract.cpp
 *
 * Enthält die Implementierungen der Klasse IbeoLaserAbstract, die als abstrakte und nicht instantiierbare Klasse für alle abgeleiteten Klassen als Bauplan dient.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 */
#include "../Additional/Defines/Defines.hpp"
#include "IbeoLaserAbstract.hpp"

#include <string>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "IbeoLaserException.hpp"

#include "../Additional/Buffers/Ringbuffer.hpp"



namespace ibeo
{

	/**
	 * Der Konstruktor der Klasse.
	 *
	 * @param[in] IP_address Die IP-Adresse, zu der die Verbindung mit dem Laserscanner aufgebaut werden soll.
	 * @param[in] port       Der Port über den die Verbindung laufen soll.
	 * @param[in] MAGICWORD  Das MagicWord, das den Anfang eine jeden Datenpaketes kennzeichnet
	 * @param[in] history    Die Größe der Historie. Von dort können die neuesten und auch ältere Daten abgerufen werden.
	 */
	IbeoLaserAbstract::IbeoLaserAbstract(const std::string& IP_address,
										  unsigned short port,
										  unsigned int MAGICWORD,
										  unsigned short headerSize,
										  unsigned int history
										)

										:
										IP_address(IP_address),
										port(port),
										MAGICWORD(MAGICWORD),
										headerSize(headerSize),
										history(history),
										isCurrendMeasuring(false),
										isConnected(false),
										scanNumber(0),
										numberOfScanPoints(0),
										tmpLaserDataAbstract(0),
										socket(0),
										tCapture(0),
										disclosureMethod(0)


	{

		ringBuffer = new buffer::Ringbuffer(TMPBUFSIZE * 3, buffer::DENIED);
		tmpBuffer = new unsigned char[TMPBUFSIZE];


	}

	/**
	 * Der Destruktor der Klasse
	 */
	IbeoLaserAbstract::~IbeoLaserAbstract()
	{
		ibeoLaserDataAvailable.disconnect_all_slots();
		stopMeasuring();
		this->disconnect();
		ringBuffer->~Ringbuffer();
		delete[] tmpBuffer;


		if(tmpLaserDataAbstract) delete tmpLaserDataAbstract;
	}

	/**
	 * Öffnet die Socket-Verbindung zu einem Laserscanner des Unternehmens ibeo
	 *
	 * @throw IbeoLaserConnectionOpenException Die Exception wird geworfen, wenn der Verbindungsaufbau zum Laserscanner fehlschlägt. Meist verursacht durch falsche IP oder falschen Port.
	 */
	void IbeoLaserAbstract::connect()
	{
		if(isConnected) return;
		boost::lock_guard<boost::mutex> lock(connectionMtx);
		boost::asio::ip::tcp::endpoint ibeoLaser(boost::asio::ip::address::from_string(IP_address), port);
		socket = new boost::asio::ip::tcp::socket(ioService);		
		socket->connect(ibeoLaser, error);
		if(error)
		{
			throw(IbeoLaserConnectionOpenException());
		}
		else isConnected = true;
	}

	/**
	 * Beendet die Verbindung zu einem Laserscanner des Unternehmens ibeo.
	 *
	 * @throw IbeoLaserConnectionCloseException Die Exception wird geworfen, wenn die Verbindung zu dem Laserscanner nicht geschlossen werden konnte.
	 */
	void IbeoLaserAbstract::disconnect()
	{
		if(isConnected)
		{
			isCurrendMeasuring = false;
			boost::lock_guard<boost::mutex> lock(connectionMtx);
			if(tCapture) tCapture->join();
			if(socket != 0)
			{
				socket->close(error);
			}
			if(error) throw(IbeoLaserConnectionCloseException());
			socket = 0;
			isConnected = false;
		}
	}

	/**
	 * Startet in einem eigenen Thread die Verarbeitung der Daten des Laserscanners.
	 */
	void IbeoLaserAbstract::startMeasuring()
	{
		if(isConnected)
		{
			boost::lock_guard<boost::mutex> lock(connectionMtx);
			isCurrendMeasuring = true;
			boost::function<void()> f = boost::bind( &IbeoLaserAbstract::threadedMeasuring, this );
			tCapture = new boost::thread(f);
			//threadedMeasuring(); // für Debugging einfach mal ohne Threading laufen lassen
		}
	}

	/**
	 * Beendet den Thread für die Verarbeitung der Daten des Laserscanners.
	 */
	void IbeoLaserAbstract::stopMeasuring()
	{
		isCurrendMeasuring = false;
		tCapture = 0;
	}

	/**
	 * Ruft die Laserscannerdaten ab. Die Methode beinhaltet eine Endlosschleife die in einem eigenen Thread läuft.
	 */
	void IbeoLaserAbstract::threadedMeasuring()
	{
		boost::lock_guard<boost::mutex> lock(connectionMtx);
		while(isCurrendMeasuring)
		{
			try
			{
				while(isCurrendMeasuring && !processMessageHeader(true)); // per Definition muss diese Methode alle Header einlesen und NICHT-SCANDATA-Pakete verwerfen. Erst dann können Scandaten eingelesen werden
					processScanDataHeader();
			}
			catch(ibeo::IbeoLaserReadException&)
			{
                std::cerr << "Es ist ein Fehler beim Lesen der Laserscannerdaten aufgetreten. Verbindung muss neu aufgebaut werden." << std::endl;
                this->stopMeasuring();
			}
		}
		tCapture = 0;
	}

	/** @brief Die Angabe einer Methode sorgt dafür, dass diese über neu eingetroffene GPS-Daten informiert wird --> z.B. für AreaScan.
	 *
	 * @param[in] Die Methode die informatiert werden soll.
	 *
	 */
	void IbeoLaserAbstract::discloseInformationTo(boost::function<void(ibeoLaserDataAbstractSmartPtr&)> disclosureMethod)
	{
		this->disclosureMethod = disclosureMethod;
	}

	/** @brief Deaktiviert wieder die Information für eine andere Methode.
	 *
	 */
	void IbeoLaserAbstract::deactivateDisclosing()
	{
		disclosureMethod = 0;
	}

	/**
	 * Gibt an ob die Verarbeitung der Daten des Laserscanners gerade läuft.
	 *
	 * @return true oder false
	 */
	bool IbeoLaserAbstract::getMeasuringStatus() const
	{
		return(isCurrendMeasuring);
	}

	/**
	 * Gibt an ob eine Verbindung zum Laserscanners aufgebaut wurde.
	 *
	 * @return true oder false
	 */
	bool IbeoLaserAbstract::getConnectionStatus() const
	{
		return(isConnected);
	}

	/**
	 * Holt die im Index angegebenen Scannerdaten und gibt diese als Kopie zurück.
	 *
	 * @throw IbeoLaserDataIndexException Die Exception wird dann geworfen, wenn der Index größer oder gleich der im Konstruktor angegebenen History ist.
	 *
	 * @param[in] index Der Index, des gewünschten Paketes von Laserscannerdaten.  Der Index ist Null-basiert. Der Index 0 enthält die neuesten Daten und je größer der Index wird, desto älter sind die Daten die abgerufen werden.
	 *
	 * @see ibeo::IbeoLaser
	 */
	ibeoLaserDataAbstractSmartPtr IbeoLaserAbstract::getDataFromIndex(unsigned int index)
	{
	    boost::lock_guard<boost::mutex> lock(appendingMtx);
		if(index >= history) throw(IbeoLaserDataIndexException());

	    return(data[index]);
	}

	/**
	 * Synchronisiert die Datenverarbeitung zum ankommenden Stream für den Fall, dass diese verloren geht.
	 *
	 * @throw RingbufferReadOverflowException Die Exception wird geworfen, wenn versucht wird mehr Daten aus dem darunterliegenden Ringpuffer zu lesen, als in ihm enthalten sind.
	 */
	void IbeoLaserAbstract::synchronizeToStream()
	{
		unsigned char *readPtrOne, *readPtrTwo;
		unsigned int readBytesOne, readBytesTwo;

		int offset = 3;
		unsigned int readBytesLeft;

		std::cerr << "Synchronisierung mit dem Datenstrom erforderlich..." << std::flush;

		for(unsigned int i = 0; i < 10; i++)
		{
			receiveData(8000);
			readBytesLeft = ringBuffer->getReadBytesLeft();

			try
			{
				ringBuffer->readBytes(&readPtrOne, &readPtrTwo, &readBytesOne, &readBytesTwo, readBytesLeft, false);
			}
			catch(buffer::RingbufferReadOverflowException&)
			{
				std::cerr << "Es wurde versucht mehr Daten zu lesen, als im Ringpuffer enthalten sind." << "[sync]"<< std::endl;
			}

			for(unsigned int nOne = 0; nOne < readBytesOne; nOne++)
			{
				if(readPtrOne[nOne] == ((MAGICWORD & (0xFF << (8 * offset))) >> (8 * offset)))
					offset--;
				else offset = 3;
				if(offset == -1)
				{
					try
					{
						ringBuffer->readBytes(&readPtrOne, &readPtrTwo, &readBytesOne, &readBytesTwo, nOne - 3, true);
					}
					catch(buffer::RingbufferReadOverflowException&)
					{
						std::cerr << "Es wurde versucht mehr Daten zu lesen, als im Ringpuffer enthalten sind." << "[syncnOne]"<< std::endl;
					}
					std::cerr << "OK!" << std::endl << std::flush;
					return;
				}
			}
			for(unsigned int nTwo = 0; nTwo < readBytesTwo; nTwo++)
			{
				if(readPtrTwo[nTwo] == ((MAGICWORD & (0xFF << (8 * offset))) >> (8 * offset)))
					offset--;
				else offset = 3;
				if(offset == -1)
				{
					try
					{
						ringBuffer->readBytes(&readPtrOne, &readPtrTwo, &readBytesOne, &readBytesTwo, readBytesOne + nTwo - 3, true);
					}
					catch(buffer::RingbufferReadOverflowException&)
					{
						std::cerr << "Es wurde versucht mehr Daten zu lesen, als im Ringpuffer enthalten sind." << "[syncnTwo]"<< std::endl;
					}
					std::cerr << "OK!" << std::endl << std::flush;
					return;
				}
			}
		}
		std::cerr << "FEHLER, Programmabbruch!" << std::endl << std::flush;
		exit(1);
	}

	/**
	 * Liest die Daten des Laserscanners über die aufgebaute Socket-Verbindung ein.
	 *
	 * @param[in] maxBytes Die maximale Anzahl an Bytes, die auf einmal gelesen werden können.
	 */
	void IbeoLaserAbstract::receiveData(unsigned int maxBytes)
	{
		// dafür sorgen, dass der Ringpuffer möglichst viele Daten vorhält

		unsigned int countBytes = ringBuffer->getBufferSize() - ringBuffer->getReadBytesLeft();
		if(countBytes > maxBytes) // wenn mehr freier Speicher vorhanden ist, als momentan eingelesen werden soll
		{
			try
			{
				countBytes = socket->read_some(boost::asio::buffer(tmpBuffer, maxBytes));
			}
			catch(...)
			{
				throw(ibeo::IbeoLaserReadException());
			}
		}
		else // es können eben nicht mehr Bytes eingelesen werden, als im Speicher noch frei sind (maxBytes wird eben gekürzt)
		{
			try
			{
				countBytes = socket->read_some(boost::asio::buffer(tmpBuffer, countBytes));
			}
			catch(...)
			{
				throw(ibeo::IbeoLaserReadException());
			}
		}

		ringBuffer->writeBytes(tmpBuffer, countBytes);
	}

	/**
	 * Hängt die gelesenen Daten des Laserscanners in die Historie ein.
	 *
	 * @param[in] dataObj Ein Pointer auf ein Objekt, welches die Scandaten des Laserscanners beinhaltet.
	 */
	void IbeoLaserAbstract::appendData(ibeoLaserDataAbstractSmartPtr dataObj)
	{
	    boost::lock_guard<boost::mutex> lock(appendingMtx);
		// Keine weitere Überprüfung der Anzahl, da im Konstruktor bereits die benötigte Menge an Objekten angelegt wurde (DUMMY)
		data.push_front(dataObj);
		data.pop_back();

	}

	/** @brief Testet die durch den Konstruktor und Typ der Klasse gewählte Konfiguration
	 *
	 * @return Bool-Wert, TRUE Konfiguration OK, FALSE Konfiguration FEHLER
	 *
	 */
	bool IbeoLaserAbstract::testConnection()
	{
		if(!isConnected)
		{
			try
			{
				this->connect();
			}
			catch(...)
			{
				this->disconnect();
				return(false);
			}
		}
        else
        {
            this->disconnect();
            try
            {
                this->connect();
            }
            catch(...)
            {
                this->disconnect();
                return(false);
            }
        }

		try
		{
			if(!processMessageHeader(false))
			{
				this->disconnect();
				return(false);
			}
			else
			{
				this->disconnect();
				return(true);
			}
		}
		catch(IbeoLaserReadException&)
		{

		}
		return(false);
	}



} /* namespace ibeo */
