/*
 * CaptureToScreen.cpp
 *
 *  Created on: 30.05.2012
 *      Author: christian
 */

#include "../Additional/Defines/Defines.hpp"

#include "CaptureToScreen.hpp"
#include <iostream>
#include <iomanip> // std::setw
#include <ios> // std::fixed

#include "../IbeoLaser/IbeoLaserDataAbstract.hpp"
#include "../IbeoLaser/IbeoLaserScanpoint.hpp"
#include "../Additional/Convert/Convert.hpp"


namespace capture
{

	CaptureToScreen::CaptureToScreen(const std::string& separator) :
			CaptureToScreenAbstract(separator)
	{

	}

	CaptureToScreen::~CaptureToScreen()
	{

	}

	void CaptureToScreen::operator<<(ibeo::IbeoLaserDataAbstract& dataObj)
	{
		std::cout << "LSR: " << dataObj.getTimeStamp() << separator << "Scannr.: " << dataObj.getScannumber() << separator << " #ScanPoints: " << dataObj.getNumberOfScanpoints() << std::endl;

		// alle Scanpunkte auch noch ausgeben?, ist eh nicht zu erkennen auf dem Bildschirm da es zu schnell vorbeiläuft
/*
		unsigned int numOfScnPts = dataObj.getNumberOfScanpoints();

		std::cout << "--------------------------------------------------------------" << std::endl;
		for(unsigned int i = 0; i < numOfScnPts; i++)
		{
			const ibeo::IbeoLaserScanpoint *sp = const_cast<ibeo::IbeoLaserDataAbstract&>(dataObj).getScanPointAt(i);
			std::cout
				<< convert::Convert::toString<unsigned char>(sp->getLayer())
				<< separator
				<< convert::Convert::toString<unsigned char>(sp->getEcho())
				<< separator
				<< std::setw(13)
				<< std::setprecision(10)
				<< std::fixed
				<< sp->getHorizontalAngle()
				<< separator
				<< sp->getRadialDistance()
				<< separator
				<< sp->getEchoPulseWidth()
				<< separator
				<< convert::Convert::toString<unsigned char>(sp->getScanFlag())
				<< std::endl;
		}
		std::cout << std::endl;
*/
	}

	/*void CaptureToScreen::operator<<(webcam::Image& image)
	{

	}*/


} /* namespace capture */
