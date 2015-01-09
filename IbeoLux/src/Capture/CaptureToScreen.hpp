/**
 * @file CaptureToScreen.hpp
 *
 * Beherbergt die Funktionalität für das Loggen von Daten die auf dem Bildschirm ausgegeben werden sollen.
 *
 * @author Christian Blumberg
 * @date 2012-06-08
 * @version 0.0.1
 * erstes Release
 */

#ifndef CAPTURETOSCREEN_HPP_
#define CAPTURETOSCREEN_HPP_

#include "CaptureToScreenAbstract.hpp"

namespace ibeo{ class IbeoLaserDataAbstract; }

namespace capture
{

/** @brief Loggen und Ausgeben von Daten auf dem Bildschirm.
 *
 * Die Klasse kapselt Funktionalitäten, die das Loggen und gleichzeitig die Ausgabe auf dem Bildschirm ermöglichen.
 */
class CaptureToScreen : public CaptureToScreenAbstract
{
public:
	CaptureToScreen(const std::string& separator);
	virtual ~CaptureToScreen();

	virtual void operator<<(ibeo::IbeoLaserDataAbstract& dataObj);
};

} /* namespace capture */
#endif /* CAPTURETOSCREEN_HPP_ */
