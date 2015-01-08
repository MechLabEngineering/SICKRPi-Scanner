/**
 * @file IbeoLUX.hpp
 *
 * Enthält Klasse IbeoLUX, die Verbindung zu einem ibeoLUX-Laserscanner aufnehmen kann und die individuellen Daten abruft und verarbeitet.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef IBEOLUX_HPP_
#define IBEOLUX_HPP_

#include <string>
#include <fstream>

#include "IbeoLaserAbstract.hpp"

namespace ibeo
{
	/** @brief Funktionen des Laserscanners IbeoLUX.
	 *
	 * Der Namespace ibeoLUX beherbergt alle Funktionalitäten, die der Laserscannr IbeoLUX des Unternehmens ibeo benötigt.
	 */
	namespace ibeoLUX
	{
		/** @brief Verbindungsklasse des IbeoLUX
		 * Die Klasse kapselt die Funktionen die nötig sind um eine Verbindung zu diesem Laserscanner herzustellen. Außerdem liest sie dessen Daten ein und verarbeitet diese.
		 */
		class IbeoLUX : public IbeoLaserAbstract
		{
		public:

			friend class IbeoLUXData;

			static void initInstance(const std::string& IP_address, unsigned short port, unsigned int history = 1);
			static IbeoLUX *getInstance();
			void releaseInstance();



		private:
			IbeoLUX(const std::string& IP_address, unsigned short port, unsigned int history = 1);
			virtual ~IbeoLUX();
			void fillWithEmptyIbeoLaserData();
			bool processMessageHeader(bool allowSynchronisation);
			void processScanDataHeader();
			unsigned long magicWord;           		// temporäres Magicword zum Markieren des Anfangs eines Paketes
			unsigned short angleTicksPerRotation;	// Anzahl Angleticks pro Rotation

			static IbeoLUX* singletonInstance;

		};

	} // namespace ibeoLUX
}/* namespace ibeo */
#endif /* IBEOLUX_HPP_ */
