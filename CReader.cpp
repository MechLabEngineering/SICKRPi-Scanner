/*
 * CReader.cpp
 * create octree live
 *
 * Created on: 07.11.2014
 * Author: Markus
*/
#include <iostream>
#include <string>
#include "CReader.h"
#include <sstream>
#include <fstream>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <list>

#include <octomap/octomap.h>
#include <octomap/OcTree.h>
#include <octomap/Pointcloud.h>
#include "time.h"
#include "ip_connection.h"
#include "brick_imu.h"

#include <boost/chrono/thread_clock.hpp>

using namespace boost::chrono;

//
#define OCTO_RES 	0.05 // in m
#define PI 3.14159265

#define SCAN_TXT "scans/"
#define CONFIG "/media/usb0/config.cfg"
#define HOST "localhost"
#define PORT 4223

#define DEBUG 1

#define MAP_SIZE_X 10
#define MAP_SIZE_Y 10

boost::mutex mtx_ypr;
boost::mutex mtx_da;

thread_clock::time_point cb_start;

std::stringstream point_stream(std::ios::in|std::ios::out);

std::fstream file;

int data_av = 0; // octree update
bool is_imu_connected;
bool is_imu_adjustment;

using std::string;
using namespace octomap;

IPConnection ipcon;
IMU imu;

/*######################################################################
## helper functions
######################################################################*/

// trim a string
const std::string trim( const std::string& s)
{
	std::string::size_type first = s.find_first_not_of( ' ');
	if( first == std::string::npos) {
		return std::string();
	}
	else {
		std::string::size_type last = s.find_last_not_of( ' '); /// must succeed
		return s.substr( first, last - first + 1);
	}
}

void debugMessage(int level, string message)
{
	if (level >= DEBUG)
		std::cerr << message << std::endl;
	return;
}

/*######################################################################
## constructor, destructor, init
######################################################################*/
CReader::CReader()
{
	tree = NULL;
	pcloud = NULL;
	scancycles = 0;
	is_imu_connected = false;
	is_imu_adjustment = false;

	octo_res = OCTO_RES;
	ip_adress = "192.168.0.1";
	port = 12002;
	layers = 1;
	convergence = 0;

	// set angle filter
	// TODO: config the scanner 
	angle_max = 0*PI/180; // scanner max 50
	angle_min = -60*PI/180; // scanner min -60

	maxmin = false;
	duration = false;
	writebt = false;
	writecsv = false;
	is_indoor = false;
	bt_path = "";

	if (readConfig())
		debugMessage(2,"config successfully readed!");
	else
		debugMessage(2, "no config file!");

	return;
}

// read config file
int CReader::readConfig()
{
	std::ifstream fh;
	std::string line;
	size_t pos = 0;
	std::string key, val;

	fh.open(CONFIG, std::ios::in);

	if (!fh.is_open()) {
		fh.open("config.cfg", std::ios::in);
		if (!fh.is_open()) {
			return FALSE;
		}
	}

	debugMessage(2, "read config...");

	while (getline(fh,line)) {
		pos = line.find("=");
		key = trim(line.substr(0,pos));
		val = trim(line.substr(pos+1));

		try {
			if (key.compare("resolution") == 0) {
				octo_res = ::atof(val.c_str());
			} else if (key.compare("ip") == 0) {
				ip_adress = val;
			} else if (key.compare("port") == 0) {
				port = atoi(val.c_str());
			} else if (key.compare("layers") == 0) {
				layers = atoi(val.c_str()) - 1;
			} else if (key.compare("maxmin") == 0) {
				if (val.compare("true") == 0)
					maxmin = true;
				else
					maxmin = false;
			} else if (key.compare("writebt") == 0) {
				if (val.compare("true") == 0)
					writebt = true;
			} else if (key.compare("btpath") == 0) {
				bt_path = val;
			} else if (key.compare("convergence") == 0) {
				convergence = atoi(val.c_str());
			} else if (key.compare("location") == 0) {
				if (val.compare("indoor") == 0) {
					is_indoor = true;
				}
			} else if (key.compare("writecsv") == 0) {
				if (val.compare("true") == 0) {
					writecsv = true;
				}
			} else if (key.compare("minangle") == 0) {
				angle_min = atoi(val.c_str())*PI/180.0;
			} else if (key.compare("maxangle") == 0) {
				angle_max = atoi(val.c_str())*PI/180.0;
			}
		} catch (...) {
			std::cout << "unkown parameter:" << key << std::endl;
		}
	}
	fh.close();
	return TRUE;
}

CReader::~CReader()
{
	// release sensor
	releaseSensor();

	if (file.is_open())
		file.close();

	usleep(200);

	// turn off imu leds
	imu_leds_off(&imu);

	// release ipcon
	imu_set_quaternion_period(&imu, 0);
	imu_set_angular_velocity_period(&imu, 0);
	imu_destroy(&imu);
	ipcon_destroy(&ipcon); // calls ipcon_disconnect internally
}

/*######################################################################
## TINKERFORGE
######################################################################*/
void cb_connected(uint8_t connect_reason, void *user_data) 
{
    // ...then trigger enumerate
    if (is_imu_connected == false)
		ipcon_enumerate(&ipcon);
	return;
}

void cb_angular_velocity(int16_t x, int16_t y, int16_t z, void *user_data)
{
	mtx_da.lock();
	//std::cout << y << std::endl;
	if (abs(x) > 50 || abs(y) > 50 || abs(z) > 50) {
		//std::cout << "adjust" << std::endl;
		is_imu_adjustment = true;
		imu_set_convergence_speed(&imu, 500);
		usleep(30000);
		imu_set_convergence_speed(&imu, 10);
		is_imu_adjustment = false;
		//std::cout << "adjust finished" << std::endl;
	} else {
		//std::cout << "b" << std::endl;
		imu_set_convergence_speed(&imu, 1);
		is_imu_adjustment = false;
	}
	mtx_da.unlock();
	return;
}


// print incoming enumeration information
void cb_enumerate(const char *uid, const char *connected_uid,
                  char position, uint8_t hardware_version[3],
                  uint8_t firmware_version[3], uint16_t device_identifier,
                  uint8_t enumeration_type, void *user_data)
{
    int is_indoor = *(int*)user_data;


    if(enumeration_type == IPCON_ENUMERATION_TYPE_DISCONNECTED) {
        printf("\n");
        return;
    }

    // check if device is an imu
    if(device_identifier == IMU_DEVICE_IDENTIFIER) {

		std::cout << "Found IMU with UID:" << uid << std::endl;

		if (is_imu_connected) {
			std::cout << "Es ist bereits eine IMU verbunden!" << std::endl;
			return;
		}

		imu_create(&imu, uid, &ipcon);

		imu_set_convergence_speed(&imu,60);

		if (!is_indoor) {

			imu_set_angular_velocity_period(&imu, 30);

			imu_register_callback(&imu,
								IMU_CALLBACK_ANGULAR_VELOCITY,
								(void *)cb_angular_velocity,
								NULL);
		}

		imu_leds_off(&imu);
		is_imu_connected = true;
	}
}


int CReader::init()
{
	tree = new OcTree (octo_res);

	// set bounding box filter
	bb_x_max = 70.0;
	bb_x_min = 0.0;
	bb_y_max = 200.0;
	bb_y_min = -200.0;

	// activate both filter
	filter_angle = true;
	filter_bbox = true;

	// init tinkerforge ------------------------------------------------
	// create IP connection

    ipcon_create(&ipcon);

    // connect to brickd
    if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		std::cout << "Could not connect to brickd!" << std::endl;
        return false;
    }

	// register connected callback to "cb_connected"
    ipcon_register_callback(&ipcon,
                            IPCON_CALLBACK_CONNECTED,
                            (void *)cb_connected,
                            &ipcon);

    // register enumeration callback to "cb_enumerate"
    ipcon_register_callback(&ipcon,
                            IPCON_CALLBACK_ENUMERATE,
                            (void*)cb_enumerate,
                            &is_indoor);
/*
	usleep(2000000);

	// register angular velocity callback to "cb_angular_velocity"
	ipcon_register_callback(&ipcon, 
							IMU_CALLBACK_ANGULAR_VELOCITY,
							(void*)cb_angular_velocity,
							NULL);
	imu_set_angular_velocity_period(&imu,100);*/

	// sleep
	usleep(5000000);

	// check if imu is connected
	if (!is_imu_connected) {
		std::cout << "Keine Verbindung zur IMU!" << std::endl;
		return false;
	}

	// set convergence after initial stage
	imu_set_convergence_speed(&imu,convergence);

	usleep(50000);

	// init sensor
	if(!initSensor()) {
		std::cout << "Keine Verbindung zum Laserscanner!" << std::endl;
		return false;
	}

	imu_leds_on(&imu);

    cb_start = thread_clock::now();

	return true;
}

int CReader::initSensor()
{
	if (!ibeo::ibeoLUX::IbeoLUX::getInstance()) {
		ibeo::ibeoLUX::IbeoLUX::initInstance(this->ip_adress, this->port, 1);
		std::cout << "Sensor searching..." << std::endl;

		laserscanner = ibeo::ibeoLUX::IbeoLUX::getInstance();

		try {
			laserscanner->connect();
			laserscanner->startMeasuring();
		} catch (...) {
			std::cout << "Sensor not found" << std::endl;
			return false;
		}
	}

	if (laserscanner->getConnectionStatus()) {
		std::cout << "Sensor connected" << std::endl;
	} else {
		return false;
	}

	if (!laserscanner->getMeasuringStatus())
		return false;

	laserscanner->ibeoLaserDataAvailable.connect(boost::bind(&CReader::newLaserData, this, _1));

	return true;
}

/*######################################################################
## get current speed
######################################################################*/
float CReader::getSpeed()
{
	float speed = 0.0;

	return speed;
}


/*######################################################################
## get position in world
######################################################################*/
int CReader::getPosition(float *x, float *y, float *z)
{
	float v = 0.0;
	float t = 0.0;
	float th = 0.0;
	int16_t wx = 0, wy = 0, wz = 0;
	uint32_t time_ms = 0;
	static thread_clock::time_point start = thread_clock::now();

	return TRUE;

	// get speed
	v = getSpeed();

	// get Angular velocity
	imu_get_angular_velocity(&imu, &wx, &wy, &wz);

	// time since last call in ms
	time_ms = duration_cast<milliseconds>(thread_clock::now() - start).count();
	t = time_ms / 1000.0;

	// calc pos
	//th = th0 + (v/(2*d))*t;

	*x = v * cos(th)*t;
	*y = v * sin(th)*t;
	*z = *z;

	start = thread_clock::now();

	return TRUE;
}

/*######################################################################
## transform octo coords to obstacle map
######################################################################*/
int CReader::transformCoord(point3d spt, int *x, int *y, int *z)
{
	*x = (int)((octo_res*roundf(spt.x()/octo_res)) / octo_res);
	*y = (int)((octo_res*roundf(spt.y()/octo_res)) / octo_res);
	*z = (int)((octo_res*roundf(spt.z()/octo_res)) / octo_res);

	return true;
}

int initMapSegment(int map[][MAP_SIZE_Y])
{
	for (int x = 0 ; x < MAP_SIZE_X ; x++) {
		for (int y = 0 ; y < MAP_SIZE_Y ; y++) {
			map[x][y] = 0;
		}
	}
	return 0;
}

int CReader::updateMapSegment()
{
	int x, y, z;
	float value;
	// check which map segment

	// filter scanpoints

	// transform coords
	for(OcTree::leaf_iterator it = tree->begin_leafs(), end = tree->end_leafs(); it!= end; ++it) { // access node, e.g.:

		point3d p = it.getCoordinate();
		value = it->getValue();

		transformCoord(p, &x, &y, &z);

	}
	// update map

	for (int x = 0 ; x < MAP_SIZE_X ; x++) {
		for (int y = 0 ; y < MAP_SIZE_Y ; y++) {
			;//map[x][y] = 0;
		}
	}

	return 0;
}

//float xAngle, yAngle, zAngle;
void CReader::storeImuData()
{
	float x, y, z, w;
	float x1 = 0.0, y1 = 0.0, z1 = 0.0, w1 = 0.0;

	//imu_get_quaternion(&imu, &x, &y, &z, &w);

	// get avg of imu data
	for (int i = 0 ; i < 5 ; i++) {
		imu_get_quaternion(&imu, &x, &y, &z, &w);
		x1 += x;
		y1 += y;
		z1 += z;
		w1 += w;
		usleep(250);
	}
	x = x1 / 5;
	y = y1 / 5;
	z = z1 / 5;
	w = w1 / 5;

	// use octomath to calc quaternion
	// y, z, x
	octomath::Quaternion q(w,x,y,z);
	octomath::Vector3 v(0.0,0.0,0.0);
	octomath::Pose6D pp(v, q);

	xAngle = pp.roll();
	yAngle = pp.pitch();
	zAngle = pp.yaw()*-1; // invert to correct mirroring

	return;
}

float xmax  = 0.0, xmin  = 0.0, ymax  = 0.0, ymin = 0.0, zmax = 0.0, zmin = 0.0;

void CReader::writeToCSV(ibeo::IbeoLaserScanpoint *scanPt)
{
	static int state = 0;
	std::string separator = "|";
	std::stringstream ss;

	if (state == 0) {

		time_t t;
		struct tm *ts;
		char buff[80];

		// build filename
		t = time(NULL);
		ts = localtime(&t);

		strftime(buff, 80, "measure_%Y_%m_%d-%H_%M_%S.csv", ts);

		ss << bt_path << buff;

		file.open(ss.str().c_str(), std::ios::out);
		state = 1;
		if (file.is_open())
			file << "Zeitstempel|Ebene|Echo|Winkel|Radius|Xwert|Ywert|EchoPulsBreite|Scanflags" << std::endl;
	}

	if (!file.is_open())
		return;

	std::time_t ts = std::time(0);

	file << ts;
	file << separator;
	file << static_cast<unsigned short>(scanPt->getLayer());
	file << separator;
	file << static_cast<unsigned short>(scanPt->getEcho());
	file << separator;
	file << scanPt->getHorizontalAngle();
	file << separator;
	file << scanPt->getRadialDistance();
	file << separator;
	file << scanPt->getXValue();
	file << separator;
	file << scanPt->getYValue();
	file << separator;
	file << scanPt->getEchoPulseWidth();
	file << separator;
	file << static_cast<unsigned short>(scanPt->getScanFlag());
	file << std::endl;
}

void CReader::newLaserData(ibeo::ibeoLaserDataAbstractSmartPtr dat)
{
	mtx_da.lock();
	int sp_count = 0, ret = 0;
	float x, y, z;
	float x_coord = 0.0, y_coord = 0.0, z_coord = 0.0;
	unsigned int scanpoints = dat->getNumberOfScanpoints();
	ibeo::IbeoLaserScanpoint *scanPt;
	ScanNode *snode = NULL;
	pcloud = new Pointcloud();
	thread_clock::time_point start = thread_clock::now();

	//thread_clock::time_point cb_stop = thread_clock::now();
	//std::cout << "duration: " << duration_cast<milliseconds>(cb_stop - cb_start).count() << " ms\n";

	// check octree
	if (tree == NULL || pcloud == NULL || is_imu_adjustment == true)
		return;

	// count scan cycles
	scancycles++;

	// clear point stream string
	//point_stream.str(std::string());

	// get IMU data
	storeImuData();

	// get position
	if ((ret = getPosition(&x_coord, &y_coord, &z_coord)) == FALSE) {
		debugMessage(1, "no position!");
	}

	// center point laserscaner
	pose6d pose (x_coord, y_coord, z_coord, xAngle, yAngle, zAngle); // center
	//writeLaserData(TRUE, &pose);

	//scanPt->CaptureToCSV("measure.csv","|");

	for (unsigned int i = 0; i < scanpoints ; i++) {
		sp_count++;

		scanPt = dat->getScanPointAt(i);

		//point_stream.setf(std::ios::fixed);
		//point_stream << std::setprecision(2) << (float)scanPt->getXValue() << ";" << (float)scanPt->getYValue() << ";"  << (float)scanPt->getZValue() << "#";

		x = (float)scanPt->getXValue();
		y = (float)scanPt->getYValue();
		z = (float)scanPt->getZValue();

		pose6d sp (x, y, z, 0, 0, 0);
		writeLaserData(FALSE, &sp);

		// check if point inside the bb
		if ((filter_bbox && x <= bb_x_max && x >= bb_x_min &&
			y <= bb_y_max && y >= bb_y_min) &&
			(filter_angle && scanPt->getHorizontalAngle() > angle_min &&
			scanPt->getHorizontalAngle() < angle_max)) {

			 if (static_cast<int>(scanPt->getLayer()) <= layers) {
				// add point to cloud
				pcloud->push_back(x,y,z);

				if (writecsv)
					writeToCSV(scanPt);
			 }
		 }

		if (maxmin) {
			if (x > xmax) xmax = x;
			if (y > ymax) ymax = y;
			if (z > zmax) zmax = z;

			if (x < xmin) xmin = x;
			if (y < ymin) ymin = y;
			if (z < zmin) zmin = z;
		}
	}

	data_av = 1;

	//std::cout << "TreeMem:" << tree->memoryUsage() << std::endl;
	//std::cout << "CPULoad:" << getCPULoad() << std::endl;

	snode = new ScanNode(pcloud,pose,1);

	tree->insertPointCloud(*snode,-1,true,false);

	mtx_da.unlock();

	// calc delay
	thread_clock::time_point stop = thread_clock::now();
	if (duration) {

		std::cout << "duration: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
	}

	cb_start = thread_clock::now();

	return;
}

// get octomap data for lds server
int CReader::getOctomap(char *buff)
{
	int node_num = 0;
	int pos = 0;
	float value;

	mtx_da.lock();
	if (data_av == 1) {

		data_av = 0;

		node_num = tree->getNumLeafNodes();
		memcpy(buff,&node_num, sizeof(int));
		pos += sizeof(int);

		// iterate octomap
		for(OcTree::leaf_iterator it = tree->begin_leafs(), end = tree->end_leafs(); it!= end; ++it) { // access node, e.g.:

			point3d p = it.getCoordinate();
			value = it->getValue();

			memcpy(buff+pos,&(p.x()), sizeof(float));
			pos += sizeof(float);
			memcpy(buff+pos,&(p.y()), sizeof(float));
			pos += sizeof(float);
			memcpy(buff+pos,&(p.z()), sizeof(float));
			pos += sizeof(float);
			memcpy(buff+pos,&value, sizeof(float));
			pos += sizeof(float);
		}

		mtx_da.unlock();
		return pos;
	}
	mtx_da.unlock();
	return 0;
}

int CReader::writeDataBT()
{
	time_t t;
	struct tm *ts;
	char buff[128];
	std::stringstream ss;

    if (!writebt)
		return 0;

    // build filename 
    t = time(NULL);
    ts = localtime(&t);

    strftime(buff, 80, "tree_%Y_%m_%d-%H_%M_%S.bt", ts);

	tree->updateInnerOccupancy();

	// lossless compression of the octree
	tree->prune();

	ss << bt_path << buff;

	tree->writeBinary(ss.str());

	if (maxmin) {
		std::cout << "----------------------------" << std::endl;
		std::cout << "xmax:" << xmax << std::endl << "xmin:" << xmin << std::endl;
		std::cout << "ymax:" << ymax << std::endl << "ymin:" << ymin << std::endl;
		std::cout << "zmax:" << zmax << std::endl << "zmin:" << zmin << std::endl;
	}

	return 0;
}

int CReader::writeLaserData(BOOL new_scan, pose6d * scanpoint)
{
		return writeTreePlain(new_scan, scanpoint);
}

int CReader::writeTreePlain(BOOL new_scan, pose6d * scanpoint)
{
	time_t t;
	struct tm *ts;
	char buff[128];
	std::stringstream ss;

    // build filename 
    t = time(NULL);
    ts = localtime(&t);

    strftime(buff, 80, "scan_%Y_%m_%d-%H_%M_%S.log", ts);

	ss << SCAN_TXT << buff;

	std::fstream f;
	f.open(ss.str().c_str(), std::ios::out | std::ios::app);
	if (f.is_open()) {
		if (new_scan)
			f << "NODE" << " " << scanpoint->x() << " " << scanpoint->y() << " " << scanpoint->z() << " " << scanpoint->roll() << " " << scanpoint->pitch() << " " << scanpoint->yaw() << "\n";
		else
			f << scanpoint->x() << " " << scanpoint->y() << " " << scanpoint->z() << "\n";
	}
	f.close();
	return 0;
}

void CReader::releaseSensor()
{
	// stop laserscanner
	laserscanner->stopMeasuring();
	usleep(1000);
	laserscanner->releaseInstance();

	// write octree binary
	if (writebt)
		writeDataBT();

	// clean up
	tree->clear();
	tree = NULL;

	return;
}

// get cpu load
int CReader::getCPULoad()
{
	int fd = 0;
	char buff[1024];
	float load;

	fd = open("/proc/loadavg", O_RDONLY);
	if (fd < 0)
		return -1;
	read(fd,buff,sizeof(buff)-1);
	sscanf(buff, "%f", &load);
	close(fd);

	return (int)(load*100);
}

// get ram load
int CReader::getRAMLoad(int* total, int* free)
{
	FILE* fp = NULL;
	char buff[256];
	char name[100], unit[16];

	if ((fp = fopen("/proc/meminfo","r")) == NULL) {
		perror("no file meminfo!");
		return 0;
	}

	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%s %d %s\n", name, total, unit);

	fgets(buff, sizeof(buff), fp);
	sscanf(buff, "%s %d %s\n", name, free, unit);

	//std::cout << "Total:" << *total << " Kb" << std::endl;
	//std::cout << "Free:"  << *free  << " Kb" <<  std::endl;
	//std::cout << "InUse:" << ((*total-*free)/1024) << " Mb"  << std::endl;

	fclose(fp);
	return 1;
}


