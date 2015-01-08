/**
 * @file IbeoLaserDataAbstract.cpp
 *
 * Enthält die Implementierungen der Klasse IbeoLaserDataAbstract, die als abstrakte und nicht instantiierbare Klasse für alle abgeleiteten Klassen als Bauplan dient.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#include <cstring>
#include <cmath>
#include "IbeoLaserDataAbstract.hpp"
#include "../Additional/Timestamp/Timestamp.hpp"
#include "../Additional/Defines/Defines.hpp"

namespace ibeo
{

	/**
	 * Der Konstruktor der Klasse
	 * @param[in] bodySize				Die Länge der Messdaten ohne den vorgelagerten Header.
	 * @param[in] scanNumber			Fortlaufende Nummer die als ID für jeden Scandurchlauf dient.
	 * @param[in] numberOfScanPoints	Die Anzahl an Scanpunkten die übermittelt werden.
	 * @param[in] scanPointSize			Die Länge in Bytes eines Scanpunktes.
	 * @param[in] scandataPointer		Der Pointer auf die Messdaten.
	 */
	IbeoLaserDataAbstract::IbeoLaserDataAbstract(	const unsigned long bodySize,
													const unsigned short scanNumber,
													const unsigned short numberOfScanPoints,
													const unsigned int scanPointSize,
													unsigned char* scandataPointer
												)

												:

												timeStamp(Timestamp::getTimeStamp()),
												bodySize(bodySize),
												scanNumber(scanNumber),
												numberOfScanPoints(numberOfScanPoints),
												scanPointSize(scanPointSize)

	{
		this->scandataPointer = new unsigned char[bodySize]; // legt einen neuen Bytebuffer an, der die Scanpunkte enthält.
		memcpy(this->scandataPointer, scandataPointer, bodySize);
	}

	IbeoLaserDataAbstract::~IbeoLaserDataAbstract()
	{
		delete[] this->scandataPointer;
	}

	/**
	 * Gibt die ID des Scans zurück. Diese wird fortlaufend erhöht.
	 * @return Die ID.
	 */
	const unsigned short& IbeoLaserDataAbstract::getScannumber() const
	{
		return(this->scanNumber);
	}

	/**
	 * Gibt die Anzahl der Scanpunkte zurück.
	 * @return Die Anzahl der Scanpunkte.
	 */
	const unsigned short& IbeoLaserDataAbstract::getNumberOfScanpoints() const
	{
		return(this->numberOfScanPoints);
	}

	/**
	 * Gibt den Timestamp zurück, der den Zeitpunkt der Generierung der Daten darstellt.
	 * @return Der Timestamp in Millisekunden.
	 */
	const u_int_64_t& IbeoLaserDataAbstract::getTimeStamp() const
	{
		return(this->timeStamp);
	}

	const unsigned char* IbeoLaserDataAbstract::getDataPointer() const
	{
		return(this->scandataPointer);
	}


} /* namespace ibeo */
