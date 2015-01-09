/**
 * @file IbeoLaserAbstract.hpp
 *
 * Enthält Klasse IbeoLaser, die als abstrakte und nicht instantiierbare Klasse für alle abgeleiteten Klassen als Bauplan dient.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 */

#ifndef IBEOLASERABSTRACT_HPP_
#define IBEOLASERABSTRACT_HPP_

#include "../Additional/Defines/Defines.hpp"

#include <string>
#include <deque>
#include <vector>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>
#include <boost/smart_ptr.hpp>

#include "IbeoLaserDataAbstract.hpp"

#include "../Additional/Buffers/Ringbuffer.hpp"
#include "../Additional/XMath/MovingMatrix.hpp"


namespace ibeo{ class IbeoLaserDataAbstract; }
//namespace vectorCalc{ class MovingMatrix; }

/** @brief Funktionen von Laserscannern von ibeo.
 *
 * Der Namespace ibeo beinhaltet die Funktionalitäten die die Laserscanner des Unternehmens ibeo beherbergen.
 */
namespace ibeo
{
#define TMPBUFSIZE 120000

	enum DATATYPE
	{ IbeoLUX_SCANDATA = 0x2202,
	  IbeoLUX_OBJECTDATA = 0x2221,
	  IbeoLUX_ERRORWARNINGS = 0x2030,
	  IbeoAlascaXT_SCANDATA = 0x0F, // in ALC.h sowie Doku AlascaXT --> Datentyp FILE_TYPE_COMPRESSED_SCAN = 15
	  IbeoAlascaXT_OBJECTDATA = 0x01,
	  UNKNOWN
	};

	typedef boost::shared_ptr<ibeo::IbeoLaserDataAbstract> ibeoLaserDataAbstractSmartPtr;

	/** @brief allgemeine Basisklasse für Laserscanner von ibeo.
	 *
	 * Die Klasse IbeoLaserAbstract dient für nachfolgende abgeleitete Klassen als Bauplan. Sie sollte und kann nicht instantiiert und muss zwingend abgeleitet werden um die Funktionalität nutzen zu können.
	 *
	 * @author Christian Blumberg
	 * @date 2012-06-06
	 *
	 * @version 0.0.1
	 * erstes Release
	 */
	class IbeoLaserAbstract
	{
	public:

		virtual void releaseInstance() = 0;
		void connect();
		void disconnect();

		bool getMeasuringStatus() const;
		bool getConnectionStatus() const;
		void startMeasuring();
		void stopMeasuring();

		void discloseInformationTo(boost::function<void(ibeoLaserDataAbstractSmartPtr&)> disclosureMethod); ///< über neue Informationen auch die folgende Methode informieren
		void deactivateDisclosing();

		bool testConnection();

		ibeoLaserDataAbstractSmartPtr getDataFromIndex(unsigned int index);

		boost::signals2::signal< void (ibeoLaserDataAbstractSmartPtr dat) > ibeoLaserDataAvailable;
		//boost::signals2::signal< void ()> imuData;


	protected:
		IbeoLaserAbstract(const std::string& IP_address,
						  unsigned short port,
						  unsigned int MAGICWORD,
						  unsigned short headerSize,
						  unsigned int history = 1);

		virtual ~IbeoLaserAbstract();

		virtual bool processMessageHeader(bool allowSynchronisation) = 0;	///< verarbeitet den Header einer Messdatenreihe
		virtual void processScanDataHeader() = 0;							///< verarbeitet die nachgelagerten Daten nach dem Header, jedoch nicht die Messdaten selbst, und legt ein passendes DataObjekt an.
		virtual void fillWithEmptyIbeoLaserData() = 0;						///< füllt die deques mit NULL-Daten (Leerdaten), so dass später einige IF-Konstrukte gespart werden können beim Abruf von DataObjekten.

		void receiveData(unsigned int maxBytes);
		void synchronizeToStream();
		void appendData(ibeoLaserDataAbstractSmartPtr dataObj);
		void threadedMeasuring();

		const std::string IP_address;					///< die IP-Adresse, mit dem sich das Objekt verbinden soll
		const unsigned short port;						///< der Port auf dem der Server des Laserscanners läuft
		const unsigned int MAGICWORD;					///< das für den jeweiligen Laserscanner verwendete MagicWord (z.B. 0xAFFEC0C0)
		const unsigned short headerSize;				///< die Größe des Headers der Messdaten dieser Laserscannertyps
		const unsigned int history;						///< die Historie der Scandaten
		unsigned long bodySize;            				///< gibt die Länge der Scandaten an, auf die scandataPointer zeigt
		u_int_64_t dataType;							///< gibt den Datentyp der Messdaten an, z.B. SCANDATA

		bool isCurrendMeasuring;						///< gibt an ob gerade eine Messung stattfindet
		bool isConnected;								///< gibt an, ob das Objekt eine Verbindung zum Laserscanner hergestellt hat

		unsigned short scanNumber;       	  			///< fortlaufende Nummer des Scanvorgangs
		unsigned short numberOfScanPoints;				///< Anzahl der übermittelten Scanpunkte im aktuellen Paket
		IbeoLaserDataAbstract* tmpLaserDataAbstract;

		boost::asio::io_service ioService;
		boost::asio::ip::tcp::socket *socket;
		boost::system::error_code error;				///< speichert Fehlernummern, die beim Verbindungsaufbau auftreten können
		buffer::Ringbuffer *ringBuffer;					///< der Ringpuffer, der die Daten vom Socket bekommt
		unsigned char *tmpBuffer;						///< temporärer Speicherplatz für Daten
		std::deque< ibeoLaserDataAbstractSmartPtr > data;		///< speichert die Historie der Scandaten
		boost::mutex connectionMtx;								///< ein Mutex
		boost::mutex appendingMtx;
		boost::mutex threadingMtx;
		boost::thread *tCapture;						///< ein Thread für die Messung

		vectorCalc::MovingMatrix zValueLayerAdjustmentMatrix[4];	///< dient zur Rotation um die y-Achse um so den z-Wert mit Hilfe der Scanebene zu errechnen
		boost::function<void(ibeoLaserDataAbstractSmartPtr)> disclosureMethod;


	};
}  // namespace ibeo
#endif /* IBEOLASERABSTRACT_HPP_ */

