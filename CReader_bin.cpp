/*
 * CReader.cpp
 * save laser data to binary file
 * 
 * Created on: 22.12.2014
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
#define PI 3.14159265

#define CONFIG "/media/usb0/config.cfg"
#define HOST "localhost"
#define PORT 4223

#define DEBUG 1

std::fstream logfile;

boost::mutex mtx_da;

int data_av = 0;
bool is_imu_connected;

using std::string;
using namespace octomap;

IPConnection ipcon;
IMU imu;

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

// constructor
CReader::CReader()
{
	scancycles = 0;
	is_imu_connected = false;
	
	ip_adress = "192.168.0.1";
	port = 12002;
	convergence = 0;
	
	// set angle filter
	angle_max = 0*PI/180; // scanner max 50
	angle_min = -60*PI/180; // scanner min -60
	
	maxmin = false;
	duration = false;
	writebt = false;
	writecsv = false;
	is_indoor = false;
	
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
			if (key.compare("ip") == 0) {
				ip_adress = val;
			} else if (key.compare("port") == 0) {
				port = atoi(val.c_str());
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
	mtx_da.lock();
	releaseSensor();
	mtx_da.unlock();
	
	usleep(200);

	// turn off imu leds
	imu_leds_off(&imu);

	// release ipcon
	imu_set_quaternion_period(&imu, 0);
	imu_set_angular_velocity_period(&imu, 0);
	imu_destroy(&imu);
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}

// TINKERFORGE

void cb_connected(uint8_t connect_reason, void *user_data) 
{
    // ...then trigger enumerate
    ipcon_enumerate(&ipcon);
}

int imu_cb = 0;
double yaw = 0, pitch = 0, roll = 0;

void cb_angular_velocity(int16_t x, int16_t y, int16_t z, void *user_data)
{
	if (abs(x) > 3 || abs(y) || abs(z)) {
		imu_set_convergence_speed(&imu, 30);
	} else {
		imu_set_convergence_speed(&imu, 0);	
	}
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
			
			imu_set_angular_velocity_period(&imu, 10);	

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

    // Connect to brickd
    if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		std::cout << "Could not connect to brickd!" << std::endl;
        return false;
    }
    
	// Register connected callback to "cb_connected"
    ipcon_register_callback(&ipcon,
                            IPCON_CALLBACK_CONNECTED,
                            (void *)cb_connected,
                            &ipcon);

    // Register enumeration callback to "cb_enumerate"
    ipcon_register_callback(&ipcon,
                            IPCON_CALLBACK_ENUMERATE,
                            (void*)cb_enumerate,
                            &is_indoor);

	ipcon_enumerate(&ipcon);
	// sleep
	usleep(7000000);
	
	// check if imu is connected
	if (!is_imu_connected) {
		std::cout << "Keine Verbindung zur IMU!" << std::endl;
		return false;
	}
	
	// set convergence after initial stage
	imu_set_convergence_speed(&imu,convergence);

	usleep(50000);
	
	logfile.open("bin/rawdata.bin", std::ios::out | std::ios::trunc);
	logfile.close();

	logfile.open("bin/rawdata.bin", std::ios::app | std::ios::binary | std::ios::out);
	
	// init sensor
	if(!initSensor()) {
		std::cout << "Keine Verbindung zum Laserscanner!" << std::endl;
		return false;
	}
	
	imu_leds_on(&imu);
                            
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
float xAngle, yAngle, zAngle;
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

void CReader::newLaserData(ibeo::ibeoLaserDataAbstractSmartPtr dat)
{
	mtx_da.lock();
	
	unsigned int scanpoints = dat->getNumberOfScanpoints();
	unsigned short scannumber = dat->getScannumber();
	
	ibeo::IbeoLaserScanpoint *scanPt;
	
	thread_clock::time_point start = thread_clock::now();
	
	// get IMU data
	storeImuData();
	
	logfile.write(reinterpret_cast<const char*>(&scanpoints), sizeof(scanpoints));
	logfile.write(reinterpret_cast<const char*>(&xAngle), sizeof(xAngle));
	logfile.write(reinterpret_cast<const char*>(&yAngle), sizeof(yAngle));
	logfile.write(reinterpret_cast<const char*>(&zAngle), sizeof(zAngle));
	
	//std::cout << "NODE " << xAngle << " " << yAngle << " " << zAngle << std::endl;

	for (unsigned int i = 0; i < scanpoints ; i++) {
		scanPt = dat->getScanPointAt(i);
		float xraw = scanPt->getXValue();
		float yraw = scanPt->getYValue();
		float zraw = scanPt->getZValue();
		//unsigned char layer = scanPt->getLayer();
		//unsigned char flag = scanPt->getScanFlag();
		//float distance = scanPt->getRadialDistance();
		//float scanAngle = scanPt->getHorizontalAngle();	
		//unsigned short intensity = scanPt->getEchoPulseWidth();
		//unsigned char echo = scanPt->getEcho();

		//logfile.write(reinterpret_cast<const char*>(&layer), sizeof(layer));     //short: 2Bytes float: 4Bytes char: 1Byte
		//logfile.write(reinterpret_cast<const char*>(&echo), sizeof(echo));
		//logfile.write(reinterpret_cast<const char*>(&scanAngle), sizeof scanAngle);
		//logfile.write(reinterpret_cast<const char*>(&distance), sizeof distance);
		logfile.write(reinterpret_cast<const char*>(&xraw), sizeof xraw);
		logfile.write(reinterpret_cast<const char*>(&yraw), sizeof yraw);
		logfile.write(reinterpret_cast<const char*>(&zraw), sizeof zraw);
		//logfile.write(reinterpret_cast<const char*>(&intensity), sizeof intensity);
		//logfile.write(reinterpret_cast<const char*>(&flag), sizeof flag);
		
		//std::cout << xraw << " " << yraw << " " << zraw << std::endl;
	}
	
	data_av = 1;
	
	// calc delay
	thread_clock::time_point stop = thread_clock::now();

	//printf("Timestamp: %llu ", timestamp); 
	printf("| Scanpoints: %d", scanpoints);
	printf("| Scannumber: %hu", scannumber); 
	printf("| Time elapsed: %ld ms \n", duration_cast<milliseconds>(stop - start).count());
	
	//releaseSensor();
	
	mtx_da.unlock();

	return;
}

// get octomap data for lds server
int CReader::getOctomap(std::stringstream& buff)
{

	return 0;
	
}

int CReader::writeDataBT()
{
	
	return 0;
}

void CReader::releaseSensor()
{
	// stop laserscanner
	laserscanner->stopMeasuring();
	usleep(1000);
	laserscanner->releaseInstance();

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


