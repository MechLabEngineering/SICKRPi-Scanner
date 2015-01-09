/**
 * @file IbeoLaserDataAbstract.hpp
 *
 * Enthält Klasse IbeoLaserDataAbstract, die als abstrakte und nicht instantiierbare Klasse für alle abgeleiteten Klassen als Bauplan dient.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef IBEOLASERDATAABSTRACT_HPP_
#define IBEOLASERDATAABSTRACT_HPP_

#include "../Additional/Defines/Defines.hpp"
#include "IbeoLaserScanpoint.hpp"

#include <boost/thread/mutex.hpp>

namespace ibeo
{
	//namespace ibeoAlascaXT {class IbeoAlascaXT;}
	namespace ibeoLUX {class IbeoLUX;}


	/** @brief allgemeine Laserscanner-Scandaten-Klasse.
	 *
	 * Die Klasse dient als allgemeiner Bauplan für Ableitungen weiterer Klassen, die die Daten verschiedener Laserscanner des Unternehmens ibeo verarbeiten müssen.
	 * Sie ist abstract und kann/sollte nicht instantiiert werden, da diese unvollständig ist und erst durch Spezialisierung komplettiert wird in ihrer Funktionalität.
	 *
	 * @author Christian Blumberg
	 * @date 2012-06-05
	 * @version 0.0.1
	 * erstes Release
	 */
	class IbeoLaserDataAbstract
	{
	public:
		//friend class ibeoAlascaXT::IbeoAlascaXT;
		friend class ibeoLUX::IbeoLUX;
		friend class IbeoLaserAbstract;
		friend class capture::CaptureToCSV;

		virtual ~IbeoLaserDataAbstract();
		virtual short int getAngleTicksPerRotation() = 0;
		virtual unsigned char * getScanPointRawAt(const unsigned int scanpointIndex) = 0;
		virtual IbeoLaserScanpoint* getScanPointAt(unsigned int scanpointIndex) = 0; ///< liefert bei abgeleiteten Klassen einen Scanpunkt zurück
		const unsigned short& getScannumber() const;
		const unsigned short& getNumberOfScanpoints() const;
		const u_int_64_t& getTimeStamp() const;
		const unsigned char* getDataPointer() const;


		/**
		 * Berechnet aus gegebenen kartesischen Koordinaten die dazugehörigen Polarkoordinaten.	//für ALASCA
		 * @param[in]  xValue 			Die kartesische X-Koordinate.
		 * @param[in]  yValue 			Die kartesische Y-Koordinate.
		 * @param[out] radialDistance	Der Radius.
		 * @param[out] angle			Der Winkel Phi.
		 */
		inline void calculatePolarCoordinates(float& xValue, float& yValue, float *radialDistance, float *angle)
		{
			*radialDistance = sqrt( (xValue * xValue) + (yValue * yValue)); // X und Y Werte sind schon in Meter, wenn vorher die Auslösungsanpassung durchgeführt wurde
			*angle = atan2(yValue, xValue); // beachtet bei der Umrechnung automatisch den Quadranten!
		}

		/**
		 * Berechnet aus gegebenen Polarkoordinaten die dazugehörigen kartesischen Koordinaten.
		 * @param[in]	radialDistance	Der Radius.
		 * @param[in]	angle			Der Winkel Phi.
		 * @param[out]	xValue			Die kartesische X-Koordinate.
		 * @param[out]	yValue			Die kartesische Y-Koordinate.
		 */
		inline void calculateCartesianCoordinates(float& radialDistance, float& angle, float* xValue, float* yValue)
		{
			*xValue = radialDistance * 0.01 * cos(angle);
			*yValue = radialDistance * 0.01 * sin(angle);
		}

	protected:
		IbeoLaserDataAbstract(	const unsigned long bodySize,
								const unsigned short scanNumber,
								const unsigned short numberOfScanPoints,
								const unsigned int scanPointSize,
								unsigned char* scanDataPointer
							 );

		const u_int_64_t timeStamp; 				///< enthält den Timestamp, vom Zeitpunkt der Erstellung des Objektes

		// Headerdaten
		const unsigned long bodySize;            			///< gibt die Länge der Scandaten in <data> an
		const unsigned short scanNumber;       	  			///< fortlaufende Nummer des Scanvorgangs
		const unsigned short numberOfScanPoints;			///< Anzahl der übermittelten Scanpunkte im aktuellen Paket
		const unsigned int scanPointSize;					///< die Größe eines Scanpunktes in dem Bytestrom
		unsigned char* scandataPointer;						///< der Bytestrom, der die Scanpunktdaten enthält


		// Scanpoint-Objekt und einzelne temporäre Daten
		IbeoLaserScanpoint scanPoint;						///< wird später als Pointer zurückgegeben und enthält die Daten eines Scanpunktes
		unsigned char scanPointRaw[10];
		unsigned char layer;								///< die Ebene, auf der der Scanpoint gemessen wurde
		unsigned char echo;									///< Echonummer dieses Scanpoints
		unsigned char scanFlag;								///< z.B. Clutter, Dirt, Ground
		short xValue;										///< kartesische Koordinate X
		short yValue;										///< kartesische Koordinate Y
		short zValue;										///< kartesische Koordinate Z, nachdem diese durch eine Rotationsmatrix berechnet wurde (Scannebene umrechnen)
		unsigned short int echoPulseWidth;					///< Echo-Pulsbreite

	};

} /* namespace ibeo */
#endif /* IBEOLASERDATAABSTRACT_HPP_ */
