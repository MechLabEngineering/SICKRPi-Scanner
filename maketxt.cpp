#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main()
{

	std::fstream logfile_bin;
	std::ofstream logfile_node;

	logfile_bin.open("bin/rawdata.bin", ios::binary | ios::in);
	logfile_node.open("rawdata.nodes", ios::trunc);

	char buffer[40]; //, buff8[8], buff2[2], buff4[4];
	unsigned int scanpoints = 0;
	float xAngle = 0, yAngle = 0, zAngle = 0;
	float xValue = 0, yValue = 0, zValue = 0;

	stringstream ss;

	//logfile_csv << "# Laserscanner: LSR: <Zeitstempel>|<Ebene>|<Echo>|<Winkel>|<Radius>|<XWert>|<YWert>|<EchoPulsBreite>|<ScanFlags>" << std::endl;

	unsigned int cycle = 0;
	while (!logfile_bin.eof())
	{

		logfile_bin.read(buffer, sizeof(scanpoints)+3*sizeof(xAngle));
		//logfile_bin.read(buff8, sizeof(timestamp));
		//logfile_bin.read(buff4, sizeof(scanpoints));

		memcpy(&(scanpoints), buffer, sizeof(scanpoints));
		memcpy(&(xAngle), buffer+sizeof(scanpoints), sizeof(xAngle));
		memcpy(&(yAngle), buffer+sizeof(scanpoints)+sizeof(yAngle), sizeof(yAngle));
		memcpy(&(zAngle), buffer+sizeof(scanpoints)+2*sizeof(zAngle), sizeof(zAngle));		

		printf(" Scanpoints %d \n", scanpoints);
		
		// NODE x y z roll pitch yaw
		ss << "NODE 0 0 0 " << xAngle << " " << yAngle << " " << zAngle << endl; 
		logfile_node  << ss.str();

		for (unsigned int i=0; i<scanpoints; i++)
		{
			logfile_bin.read(buffer, 3*sizeof(float));

			//memcpy(&(echo), buffer, 1);  					
			//memcpy(&(layer), buffer+2, 1);
			//memcpy(&(scanAngle), buffer+4, sizeof(float));
			//memcpy(&(distance), buffer+4+sizeof(float), sizeof(float));
			memcpy(&(xValue), buffer, sizeof(float));
			memcpy(&(yValue), buffer+sizeof(float), sizeof(float));
			memcpy(&(zValue), buffer+2*sizeof(float), sizeof(float));
			//memcpy(&(intensity), buffer+4+4*sizeof(float), 2);
			//memcpy(&(flag), buffer+6+4*sizeof(float), 1);

			if (logfile_bin.eof())
			{
				logfile_bin.close();
				logfile_node.close();
				return 0;
			}

			ss.str("");
			ss <<  xValue << " " << yValue << " " << zValue << endl;
			logfile_node  << ss.str();
		}

		cycle++;

	}

	logfile_bin.close();
	logfile_node.close();

	return 0;

}
