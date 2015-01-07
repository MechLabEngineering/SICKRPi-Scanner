#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "boost/math/constants/constants.hpp"
#include "IbeoLux/src/IbeoLaser/IbeoLUX.hpp"
#include "IbeoLux/src/IbeoLaser/IbeoLaserScanpoint.hpp"
#include "IbeoLux/src/IbeoLaser/IbeoLaserAbstract.hpp"
#include "IbeoLux/src/Additional/XMath/MovingMatrix.hpp"
#include "IbeoLux/src/Additional/XMath/Convert.hpp"

namespace ibeo{
	class IbeoLaserAbstract;
	class IbeoLaserDataAbstract;
	namespace ibeoLUX  {class IbeoLUX;}
 }

using namespace std;

int main()
{

	std::fstream logfile_bin;
	std::ofstream logfile_node;

	logfile_bin.open("rawdata.bin", ios::binary | ios::in);
	logfile_node.open("rawdata.nodes", ios::trunc);

	char buffer[40]; //, buff8[8], buff2[2], buff4[4];
	ibeo::IbeoLaserScanpoint scanPoint;
	unsigned int scanpoints = 0;
	float xAngle = 0, yAngle = 0, zAngle = 0;
	float xValue = 0, yValue = 0, zValue = 0;
	unsigned char layer;		///< die Ebene, auf der der Scanpoint gemessen wurde
	unsigned char echo;		///< Echonummer dieses Scanpoints
	unsigned char scanFlag;

	short int angleTicksPerRotation;	///< Anzahl Angleticks pro Rotation
	unsigned short int echoPulseWidth;
	short int horizontalAngleTicks;				///< die horizontalen AngleTicks
	float horizontalAngle;							///< der horizontale Winkel
	float radialDistance;			
	
	vectorCalc::MovingMatrix zValueLayerAdjustmentMatrix[4];	///< dient zur Rotation um die y-Achse um so den z-Wert mit Hilfe der Scanebene zu errechnen				///< der Radius

	stringstream ss;
	
	float angle = -1.2;
	for(unsigned int i = 0; i < 4; i++, angle += 0.8)
	{
		zValueLayerAdjustmentMatrix[i].rotateY(MathConvert::Convert::deg2rad(angle));
	}

	//logfile_csv << "# Laserscanner: LSR: <Zeitstempel>|<Ebene>|<Echo>|<Winkel>|<Radius>|<XWert>|<YWert>|<EchoPulsBreite>|<ScanFlags>" << std::endl;

	unsigned int cycle = 0;
	while (!logfile_bin.eof())
	{

		logfile_bin.read(buffer, sizeof(scanpoints)+3*sizeof(xAngle)+sizeof(angleTicksPerRotation));
		//logfile_bin.read(buff8, sizeof(timestamp));
		//logfile_bin.read(buff4, sizeof(scanpoints));

		memcpy(&(scanpoints), buffer, sizeof(scanpoints));
		memcpy(&(xAngle), buffer+sizeof(scanpoints), sizeof(xAngle));
		memcpy(&(yAngle), buffer+sizeof(scanpoints)+sizeof(yAngle), sizeof(yAngle));
		memcpy(&(zAngle), buffer+sizeof(scanpoints)+2*sizeof(zAngle), sizeof(zAngle));		
		memcpy(&(angleTicksPerRotation), buffer+sizeof(scanpoints)+3*sizeof(zAngle) ,sizeof(angleTicksPerRotation));

		printf(" Scanpoints %d \n", scanpoints);
		
		// NODE x y z roll pitch yaw
		ss << "NODE 0 0 0 " << xAngle << " " << yAngle << " " << zAngle << endl; 
		//cout << ss.str();
		logfile_node  << ss.str();

		for (unsigned int i=0; i<scanpoints; i++)
		{
			//logfile_bin.read(buffer, 3*sizeof(float));
			logfile_bin.read(buffer, 8);

			// Layer einlesen			Ethernet Manual - ibeo LUX scan data: Data type 0x2202
			memcpy(&(echo), buffer, 1);	//uint4
			echo >>= 4;

			// Echo einlesen
			memcpy(&(layer), buffer, 1);	//uint4
			layer &= 0x03;

			// Flags einlesen
			memcpy(&(scanFlag), buffer + 1, 1);
			scanFlag &= 0x0F;

			// Horizontalwinkel einlesen
			memcpy(&(horizontalAngleTicks), buffer + 2, 2);
			horizontalAngle = ((2.0 * boost::math::constants::pi<float>() * static_cast<float>(horizontalAngleTicks)) / static_cast<float>(angleTicksPerRotation)); // laut Doku Ethernet

			// Entfernung Laser/Messpunkt einlesen
			unsigned short int tmpShort;
			memcpy(&(tmpShort), buffer + 4, 2);
			radialDistance = static_cast<float>(tmpShort);

			// Echopulsebreite einlesen
			memcpy(&(echoPulseWidth), buffer + 6, 2);

			xValue = radialDistance * 0.01 * cos(horizontalAngle);
			yValue = radialDistance * 0.01 * sin(horizontalAngle);
			//ibeo::IbeoLaserDataAbstract::calculateCartesianCoordinates(radialDistance, horizontalAngle, &xValue, &yValue);

            //zValue = ibeo::ibeoLUX::IbeoLUX::getInstance()->zValueLayerAdjustmentMatrix[layer](2, 0) * xValue; // z-Ebene mit 0 initialisieren
			zValue = zValueLayerAdjustmentMatrix[layer](2, 0) * xValue; // z-Ebene mit 0 initialisieren
			scanPoint.setData(layer, echo, scanFlag, horizontalAngle, radialDistance, xValue, yValue, zValue, echoPulseWidth, (unsigned char*)buffer);

			if (logfile_bin.eof())
			{
				logfile_bin.close(); 
				logfile_node.close();
				return 0;
			}

			ss.str("");
			ss <<  scanPoint.getXValue() << " " << scanPoint.getYValue() << " " << scanPoint.getZValue() << endl;
			logfile_node  << ss.str();
			//cout << ss.str();
		}

		cycle++;

	}

	logfile_bin.close();
	logfile_node.close();

	return 0;

}
