/**
 * @file IbeoLUXData.hpp
 *
 * Enthält Klasse IbeoLUXData, die die Daten des Laserscanners ibeoLUX verarbeitet.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef IBEOLASERDATA_H_
#define IBEOLASERDATA_H_

#include "IbeoLaserDataAbstract.hpp"
#include "IbeoLaserScanpoint.hpp"

namespace ibeo
{
	namespace ibeoLUX
	{
		/** @brief Verarbeitung von IbeoLUX-Scandaten.
		 *
		 * Diese Klasse kapselt die Funktionalitäten, die für die Verarbeitung der Scandaten des Laserscanners IbeoKUX gebraucht werden.
		 */
		class IbeoLUXData : public IbeoLaserDataAbstract
		{
		public:

			IbeoLUXData();
			IbeoLUXData(const unsigned long bodySize,
						const unsigned short scanNumber,
						const unsigned short numberOfScanPoints,
						const unsigned short angleTicksPerRotation,
						unsigned char* scanDataPointer
					   );

			virtual ~IbeoLUXData();


//			IbeoLUXData(const IbeoLUXData& copyOf);
			short int getAngleTicksPerRotation();
			unsigned char * getScanPointRawAt(const unsigned int scanpointIndex);
			IbeoLaserScanpoint * getScanPointAt(const unsigned int scanpointIndex);

//			IbeoLUXData& operator=(const IbeoLUXData& obj);
//
//			void setData(	unsigned long magicWord,
//							unsigned long bodySize,
//							DATATYPE dataType,
//							unsigned long long timeStamp,
//							unsigned short scanNumber,
//							unsigned short numberOfScanPoints,
//							unsigned int scanPointSize,
//							unsigned short angleTicksPerRotation,
//							unsigned char* data
//						);




		private:
			const unsigned short angleTicksPerRotation;	///< Anzahl Angleticks pro Rotation
			short int horizontalAngleTicks;				///< die horizontalen AngleTicks
			float horizontalAngle;						///< der horizontale Winkel
			float radialDistance;			///< der Radius

		};
	}

} /* namespace ibeo */
#endif /* IBEOLASERDATA_H_ */
