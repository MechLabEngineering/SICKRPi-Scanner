/**
 * @file ProcessData.hpp
 *
 * Enthält die abstrakte Klasse ProcessData, die nur als abgeleitete Klasse genutzt werden sollte. Durch die überladenen Operatoren wird die Kindklasse über neu generierte Daten informiert, die diese dann auswerten kann.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#ifndef PROCESSDATA_HPP_
#define PROCESSDATA_HPP_

#include "ProcessAndCaptureAbstract.hpp"

namespace capture
{

/** @brief abgeleitete Klassen über neue Scandaten informieren.
 *
 * Die Klasse informiert die Kindklasse über neu generierte Daten, die diese dann auswerten kann.
 */
class ProcessData : public ProcessAndCaptureAbstract
{
protected:
	ProcessData() : ProcessAndCaptureAbstract("|") {}
	virtual ~ProcessData() {}

	virtual void operator<<(ibeo::IbeoLaserDataAbstract& dataObj) = 0;	    	///< Wird aufgerufen, wenn neue CAN-Bus-Daten von einem der Laserscanner vorhanden sind.

};

} /* namespace capture */
#endif /* PROCESSDATA_HPP_ */
