#ifndef _CREADER_H_
#define _CREADER_H_
#pragma once

#include <string>
#include <sstream>
#include "IbeoLux/src/Capture/ProcessData.hpp"
#include "IbeoLux/src/IbeoLaser/IbeoLUX.hpp"
#include "IbeoLux/src/IbeoLaser/IbeoLaserAbstract.hpp"
#include "IbeoLux/src/IbeoLaser/IbeoLaserException.hpp"
#include "IbeoLux/src/IbeoLaser/IbeoLaserScanpoint.hpp"
#include "boost/signals2.hpp"
#include "boost/thread.hpp"
#include "ip_connection.h"
#include "brick_imu.h"

#include <octomap/octomap.h>
#include <octomap/OcTree.h>
#include <octomap/Pointcloud.h>

#define FALSE	0
#define TRUE	1
#define BOOL	int

namespace ibeo{
	class IbeoLaserAbstract;
	class IbeoLaserDataAbstract;
	namespace ibeoLUX  {class IbeoLUX;}
 }

using std::string;
using namespace octomap;

class CReader
{
	public:
		CReader();
		~CReader();
		int init();
		int getOctomap(std::stringstream& buff);
		int writeDataBT();
		//char* printState() { return "ok"; }
	private:
		string ip_adress;
		unsigned short port;
		float octo_res;
		int layers;
		ibeo::IbeoLaserAbstract *laserscanner;

		int64_t scancycles;

		OcTree *tree;
		Pointcloud *pcloud;
		
	private:
		float bb_x_max;
		float bb_x_min;
		float bb_y_max;
		float bb_y_min;
		float bb_z_max;
		float bb_z_min;
		
		float angle_max;
		float angle_min;
		
		float filter_angle;
		float filter_bbox;
		
	private:
		int maxmin;
		int duration;
		int writebt;
		int writecsv;
		int convergence;
		int is_indoor;

	private:
		int initSensor();
		int readConfig();
		int getCPULoad();
		int getRAMLoad(int* total, int* free);
		int writeLaserData(BOOL new_scan, ibeo::IbeoLaserScanpoint * scanPt);
		void writeToCSV(ibeo::IbeoLaserScanpoint *scanPt);
		void releaseSensor();
		void storeImuData();
		void newLaserData(ibeo::ibeoLaserDataAbstractSmartPtr dat);
		
};

#endif
