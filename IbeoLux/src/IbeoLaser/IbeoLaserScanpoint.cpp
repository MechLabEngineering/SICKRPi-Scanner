/**
 * @file IbeoLaserScanpoint.cpp
 *
 * Enthält die Implementierungen der Klasse IbeoLaserScanpoint, die einen Scanpunkt eines Laserscanners repräsentiert.
 *
 * @author Christian Blumberg
 * @date 2012-06-05
 */

#include "IbeoLaserScanpoint.hpp"

namespace ibeo
{

	IbeoLaserScanpoint::IbeoLaserScanpoint() {}

	IbeoLaserScanpoint::IbeoLaserScanpoint(	const unsigned char& layer,
											const unsigned char& echo,
											const unsigned char& scanFlag,
											const float& horizontalAngle,
											const float& radialDistance,
											const float& xValue,
											const float& yValue,
											const float& zValue,
											const unsigned short& echoPulseWidth,
											unsigned char* datapointer
							  	  	  	  )
	{
		this->setData(layer, echo, scanFlag, horizontalAngle, radialDistance, xValue, yValue, zValue, echoPulseWidth, datapointer);
	}

	void IbeoLaserScanpoint::setData(	const unsigned char& layer,
										const unsigned char& echo,
										const unsigned char& scanFlag,
										const float& horizontalAngle,
										const float& radialDistance,
										const float& xValue,
										const float& yValue,
										const float& zValue,
										const unsigned short& echoPulseWidth,
										unsigned char* datapointer
									)
	{
		this->layer = layer;
		this->echo = echo;
		this->scanFlag = scanFlag;
		this->horizontalAngle = horizontalAngle;
		this->radialDistance = radialDistance;
		this->xValue = xValue;
		this->yValue = yValue;
		this->zValue = zValue;
		this->echoPulseWidth = echoPulseWidth;
		this->datapointer = datapointer;
	}

	const unsigned char& IbeoLaserScanpoint::getLayer() const
	{
		return(this->layer);
	}

	const unsigned char& IbeoLaserScanpoint::getEcho() const
	{
		return(this->echo);
	}

	const unsigned char& IbeoLaserScanpoint::getScanFlag() const
	{
		return(this->scanFlag);
	}

	const float& IbeoLaserScanpoint::getHorizontalAngle() const
	{
		return(this->horizontalAngle);
	}

	const float& IbeoLaserScanpoint::getRadialDistance() const
	{
		return(this->radialDistance);
	}

	const float& IbeoLaserScanpoint::getXValue() const
	{
		return(this->xValue);
	}

	const float& IbeoLaserScanpoint::getYValue() const
	{
		return(this->yValue);
	}

	const float& IbeoLaserScanpoint::getZValue() const
	{
		return(this->zValue);
	}

	const unsigned short& IbeoLaserScanpoint::getEchoPulseWidth() const
	{
		return(this->echoPulseWidth);
	}

} /* namespace ibeo */
