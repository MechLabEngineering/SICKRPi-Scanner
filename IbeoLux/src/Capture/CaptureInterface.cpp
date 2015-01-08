/*
 * CaptureInterface.cpp
 *
 *  Created on: 01.06.2012
 *      Author: christian
 */

#include "CaptureInterface.hpp"
#include <list>

namespace capture
{


void CaptureInterface::startProcessingOrCapturing()
{
	isCurrentCapturing = true;
}

void CaptureInterface::stopProcessingOrCapturing()
{
	isCurrentCapturing = false;
}

const bool& CaptureInterface::getProcessingOrCapturingWithStatus() const
{
	return(this->isCurrentCapturing);
}

void CaptureInterface::addProcessingOrCapturingObject(capture::ProcessAndCaptureAbstract* captureObj)
{
	this->captureObjects.push_back(captureObj);
}

void CaptureInterface::removeProcessingOrCapturingObject(capture::ProcessAndCaptureAbstract* captureObj)
{
	this->captureObjects.remove(captureObj);
}

} /* namespace capture */
