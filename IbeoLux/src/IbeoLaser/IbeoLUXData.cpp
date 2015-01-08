/**
 * @file IbeoLUXData.cpp
 *
 * Enthält die Implementierungen der Klasse IbeoLUXData, die die Daten des Laserscanners ibeoLUX verarbeitet.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#include <cstring>
#include <iostream>

#include <boost/math/constants/constants.hpp>

#include "IbeoLaserDataAbstract.hpp"
#include "IbeoLUXData.hpp"
#include "IbeoLUX.hpp"
#include "IbeoLaserException.hpp"
#include "../Additional/XMath/Vector3.hpp"


namespace ibeo
{
	namespace ibeoLUX
	{
		/**
		 * Der Blind-Konstruktor der Klasse. Dieser erzeugt einfach einen leeren Datensatz. Er wird von der Klasse IbeoLUX im Konstruktor gerufen.
		 * @see IbeoLUX
		 */
		IbeoLUXData::IbeoLUXData() : IbeoLaserDataAbstract(0, 0, 0, 0, 0),
									 angleTicksPerRotation(0),
									 horizontalAngleTicks(0),
									 horizontalAngle(0),
									 radialDistance(0)
		{

		}

		/**
		 * Der Konstruktor der Klasse.
		 * @param[in] bodySize				Die Länge der Messdaten ohne den vorgelagerten Header.
		 * @param[in] scanNumber			Fortlaufende Nummer die als ID für jeden Scandurchlauf dient.
		 * @param[in] numberOfScanPoints	Die Anzahl an Scanpunkten die übermittelt werden.
		 * @param[in] angleTicksPerRotation Die AngleTicks per Rotation???
		 * @param[in] scanPointSize			Die Länge in Bytes eines Scanpunktes.
		 * @param[in] scandataPointer		Der Pointer auf die Messdaten.
		 */
		IbeoLUXData::IbeoLUXData(	const unsigned long bodySize,
									const unsigned short scanNumber,
									const unsigned short numberOfScanPoints,
									const unsigned short angleTicksPerRotation,
									unsigned char* scandataPointer
								)

								:

								IbeoLaserDataAbstract(bodySize, scanNumber, numberOfScanPoints, 10, scandataPointer),
								angleTicksPerRotation(angleTicksPerRotation)


//									magicWord(magicWord), bodySize(bodySize), dataType(dataType), timeStamp(timeStamp),	scanNumber(scanNumber),
//									numberOfScanPoints(numberOfScanPoints), scanPointSize(scanPointSize), angleTicksPerRotation(angleTicksPerRotation), data(0)
		{

		}

//		IbeoLUXData::IbeoLUXData(const IbeoLUXData& copyOf)
//		{
//			magicWord = copyOf.magicWord;
//			bodySize = copyOf.bodySize;
//			dataType = copyOf.dataType;
//			timeStamp = copyOf.timeStamp;
//			scanNumber = copyOf.scanNumber;
//			numberOfScanPoints = copyOf.numberOfScanPoints;
//			scanPointSize = copyOf.scanPointSize;
//			angleTicksPerRotation = copyOf.angleTicksPerRotation;
//			layer = copyOf.layer;
//			echo = copyOf.echo;
//			scanFlag = copyOf.scanFlag;
//			horizontalAngleTicks = copyOf.horizontalAngleTicks;
//			horizontalAngle = copyOf.horizontalAngle;
//			radialDistance = copyOf.radialDistance;
//			echoPulseWidth = copyOf.echoPulseWidth;
//
//			this->data = new unsigned char[15000];
//			memcpy(this->data, copyOf.data, bodySize);
//		}

		/**
		 * Der Destruktor der Klasse.
		 */
		IbeoLUXData::~IbeoLUXData()
		{

		}
		
		short int IbeoLUXData::getAngleTicksPerRotation()
		{
			return angleTicksPerRotation;
		}
		
		unsigned char * IbeoLUXData::getScanPointRawAt(const unsigned int scanpointIndex)
		{
			if(scanpointIndex >= this->numberOfScanPoints)
				throw(IbeoLaserScanpointIndexException());

			unsigned char* dataPointer = this->scandataPointer; // +44

			// auskommentiert, da sowieso nur SCANDATA abgelegt werden im Speicher
			//if(this->dataType != SCANDATA) return(0);

			// den Pointer soweit versetzen, dass es dem angeforderten Index entspricht
			dataPointer += scanPointSize * scanpointIndex;

			// Rohdaten einlesen
			//memcpy(scanPointRaw, &angleTicksPerRotation,2);
			memcpy(scanPointRaw, dataPointer, 8);
			
			return scanPointRaw;
		}

		/**
		 * Gibt den durch den scanpointIndex angegebenen ScanPoint zurück.
		 * @param[in] scanpointIndex Der Index des Scanpunktes, der zurückgegeben werden soll. Dieser ist Null-basiert.
		 * @return Ein Scanpunkt eines Laserscanners.
		 */
		IbeoLaserScanpoint * IbeoLUXData::getScanPointAt(const unsigned int scanpointIndex)
		{

			if(scanpointIndex >= this->numberOfScanPoints)
				throw(IbeoLaserScanpointIndexException());

			unsigned char* dataPointer = this->scandataPointer; // +44

			// auskommentiert, da sowieso nur SCANDATA abgelegt werden im Speicher
			//if(this->dataType != SCANDATA) return(0);

			// den Pointer soweit versetzen, dass es dem angeforderten Index entspricht
			dataPointer += scanPointSize * scanpointIndex;

			// Layer einlesen			Ethernet Manual - ibeo LUX scan data: Data type 0x2202
			memcpy(&(echo), dataPointer, 1);	//uint4
			echo >>= 4;

			// Echo einlesen
			memcpy(&(layer), dataPointer, 1);	//uint4
			layer &= 0x03;

			// Flags einlesen
			memcpy(&(scanFlag), dataPointer + 1, 1);
			scanFlag &= 0x0F;

			// Horizontalwinkel einlesen
			memcpy(&(horizontalAngleTicks), dataPointer + 2, 2);
			horizontalAngle = ((2.0 * boost::math::constants::pi<float>() * static_cast<float>(horizontalAngleTicks)) / static_cast<float>(angleTicksPerRotation)); // laut Doku Ethernet

			// Entfernung Laser/Messpunkt einlesen
			unsigned short int tmpShort;
			memcpy(&(tmpShort), dataPointer + 4, 2);
			radialDistance = static_cast<float>(tmpShort);

			// Echopulsebreite einlesen
			memcpy(&(echoPulseWidth), dataPointer + 6, 2);

			float xValueDBL, yValueDBL, zValueDBL;

			IbeoLaserDataAbstract::calculateCartesianCoordinates(radialDistance, horizontalAngle, &xValueDBL, &yValueDBL);

            zValueDBL = IbeoLUX::getInstance()->zValueLayerAdjustmentMatrix[layer](2, 0) * xValueDBL; // z-Ebene mit 0 initialisieren

			scanPoint.setData(layer, echo, scanFlag, horizontalAngle, radialDistance, xValueDBL, yValueDBL, zValueDBL, echoPulseWidth, dataPointer);
			return(&(this->scanPoint));
		}

/*
//		IbeoLUXData& IbeoLUXData::operator=(const IbeoLUXData& obj)
//		{
//			if(this == &obj) return(*this);
//
//			magicWord = obj.magicWord;
//			bodySize = obj.bodySize;
//			dataType = obj.dataType;
//			timeStamp = obj.timeStamp;
//			scanNumber = obj.scanNumber;
//			numberOfScanPoints = obj.numberOfScanPoints;
//			scanPointSize = obj.scanPointSize;
//			angleTicksPerRotation = obj.angleTicksPerRotation;
//			layer = obj.layer;
//			echo = obj.echo;
//			scanFlag = obj.scanFlag;
//			horizontalAngleTicks = obj.horizontalAngleTicks;
//			horizontalAngle = obj.horizontalAngle;
//			radialDistance = obj.radialDistance;
//			echoPulseWidth = obj.echoPulseWidth;
//
//			if(this->data != 0) delete[] this->data;
//			this->data = new unsigned char[15000];
//			memcpy(this->data, obj.data, bodySize);
//
//			return(*this);
//		}

//		void IbeoLUXData::setData(	unsigned long magicWord,
//										unsigned long bodySize,
//										DATATYPE dataType,
//										unsigned long long timeStamp,
//										unsigned short scanNumber,
//										unsigned short numberOfScanPoints,
//										unsigned int scanPointSize,
//										unsigned short angleTicksPerRotation,
//										unsigned char* data
//								   )
//		{
//			this->magicWord = magicWord;
//			this->bodySize = bodySize;
//			this->dataType = dataType;
//			this->timeStamp = timeStamp;
//			this->scanNumber = scanNumber;
//			this->numberOfScanPoints = numberOfScanPoints;
//			this->scanPointSize = scanPointSize;
//			this->angleTicksPerRotation = angleTicksPerRotation;
//			memcpy(this->data, data, bodySize);
//		}

*/



	}

} /* namespace ibeo */
