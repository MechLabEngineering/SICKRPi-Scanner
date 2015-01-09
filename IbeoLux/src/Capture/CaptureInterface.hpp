/**
 * @file CaptureInterface.hpp
 *
 * Enthält Klasse CaptureInterface, die für abgeleitete Klassen die Funktionalität des Capturings in verschiedenen Formaten bereitstellt.
 *
 * @author Christian Blumberg
 * @date 2012-06-04
 */

#ifndef CAPTUREINTERFACE_HPP_
#define CAPTUREINTERFACE_HPP_

#include <list>
#include "ProcessAndCaptureAbstract.hpp"
//#include "../IbeoLaser/IbeoLaserDataAbstract.hpp"

namespace ibeo{ class IbeoLaserDataAbstract; }

namespace capture
{

/** @brief ermöglicht jeder abgeleiteten Klassen das Capturing.
 *
 * Alle Klassen, die von diesem Interface erben, können dazu genutzt werden Daten in eine Datei in jeglichem Format zu schreiben.
 */
class CaptureInterface
{
public:
	void startProcessingOrCapturing();
	void stopProcessingOrCapturing();
	void addProcessingOrCapturingObject(capture::ProcessAndCaptureAbstract* captureObj);
	void removeProcessingOrCapturingObject(capture::ProcessAndCaptureAbstract* captureObj);
	const bool& getProcessingOrCapturingWithStatus() const;

protected:

	void informAllProcessingOrCapturingObjects(ibeo::IbeoLaserDataAbstract& obj)
	{
		if(isCurrentCapturing)
		{
			std::list< capture::ProcessAndCaptureAbstract* >::iterator itEnd = this->captureObjects.end();
			for(std::list< capture::ProcessAndCaptureAbstract* >::iterator itRun = this->captureObjects.begin(); itRun != itEnd; ++itRun)
			{
				(*itRun)->operator <<(obj);
			}
		}
	}


protected:
	CaptureInterface() :
		isCurrentCapturing(false)
	{

	};

	virtual ~CaptureInterface(){};

	bool isCurrentCapturing;
	std::list< capture::ProcessAndCaptureAbstract* > captureObjects;

};

} /* namespace capture */
#endif /* CAPTUREINTERFACE_HPP_ */
