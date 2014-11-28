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

#define CONFIG "/media/usb0/config.cfg"
#define HOST "localhost"
#define PORT 4223

#define DEBUG 1
#define THREAD_STOP 0
#define THREAD_RUN  1

#define MAX_SCAN_NODES 30
#define SKIP_MAX 10
#define SKIP_MIN 0

boost::mutex mtx_ypr;
boost::mutex mtx_da;

thread_clock::time_point cb_start;

std::stringstream point_stream(std::ios::in|std::ios::out);

std::fstream file;

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
	tree = NULL;
	pcloud = NULL;
	scancycles = 0;
	is_imu_connected = false;
	
	octo_res = OCTO_RES;
	ip_adress = "192.168.0.1";
	port = 12002;
	layers = 1;
	convergence = 0;
	state = 0;
	skip_counter = 0;
	
	// set angle filter
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
	
	// start octo worker thread
	state = THREAD_RUN;
	boost::function<void()> f = boost::bind( &CReader::octoWorker, this );
	tworker = new boost::thread(f);
	
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

std::list<ScanNode> scan_nodes;

void CReader::octoWorker()
{
	//ScanNode snode; // Iterator definieren
	thread_clock::time_point cb_start; 
	
	// work as long as necessary	
	while (state == THREAD_RUN) {
		// check if scan data available
		if (scan_nodes.size() > 0) {
			cb_start = thread_clock::now(); 
			// insert scan
			debugMessage(2, "Worker: next scan node");
			//snode = scan_nodes.front();

			tree->insertPointCloud(scan_nodes.front(),-1,true,false);

			mtx_da.lock();
			scan_nodes.pop_front();
			mtx_da.unlock();
			std::cout << "Worker: " << duration_cast<milliseconds>(thread_clock::now() - cb_start).count() << " ms\n";
		
		} else {
			//debugMessage(2, "Worker: no more nodes");
		}
	}
	debugMessage(2, "Worker: Stop");
	return;
}

void CReader::newLaserData(ibeo::ibeoLaserDataAbstractSmartPtr dat)
{
	std::cout << "Callback: call" << std::endl;
	
	int sp_count = 0;
	static int sc = 0;
	float x, y, z;
	unsigned int scanpoints = dat->getNumberOfScanpoints();
	ibeo::IbeoLaserScanpoint *scanPt;
	ScanNode *snode = NULL;
	pcloud = new Pointcloud();	
	thread_clock::time_point start = thread_clock::now();
	
	// check octree
	if (tree == NULL || pcloud == NULL || state == 0)
		return;

	// count scan cycles
	scancycles++;
	
	// center point laserscaner 
	//ibeo::IbeoLaserScanpoint center(0,0,0,0.0,0.0,0.0,0.0,0.0,0,NULL);
	//writeLaserData(TRUE, &center);

	// clear point stream string
	point_stream.str(std::string());

	// get IMU data
	storeImuData();

	//scanPt->CaptureToCSV("measure.csv","|");

	for (unsigned int i = 0; i < scanpoints ; i++) {
		sp_count++;
		
		scanPt = dat->getScanPointAt(i);
		
		/*
		if (i==0) {
			std::cout << ((int)std::abs(scanPt->getHorizontalAngle()*10000))+(scanPt->getRadialDistance()) << std::endl;
		}
		*/

		//writeLaserData(FALSE, scanPt);
		//point_stream.setf(std::ios::fixed);
		//point_stream << std::setprecision(2) << (float)scanPt->getXValue() << ";" << (float)scanPt->getYValue() << ";"  << (float)scanPt->getZValue() << "#";
		
		x = (float)scanPt->getXValue();
		y = (float)scanPt->getYValue();
		z = (float)scanPt->getZValue();
	
		//point3d p3 (x,y,z);

		//p3 = p3.rotate_IP(90,90,0);
		//std::cout << scanPt->getHorizontalAngle() << "::" << angle_min << "::" << angle_max << std::endl;
		// check if point inside the bb
		if ((filter_bbox && x <= bb_x_max && x >= bb_x_min &&
			y <= bb_y_max && y >= bb_y_min) &&
			(filter_angle && scanPt->getHorizontalAngle() > angle_min &&
			scanPt->getHorizontalAngle() < angle_max)) {
			 
			 if (static_cast<int>(scanPt->getLayer()) <= layers) {
				// add point to cloud
				pcloud->push_back(x,y,z);

				//std::cout <<  scanPt->getHorizontalAngle() << std::endl;
				//std::cout <<  static_cast<int>(scanPt->getLayer()) << "::" << layers << std::endl;
				//std::cout << std::setprecision(2) << (float)scanPt->getXValue() << ";" << (float)scanPt->getYValue() << ";"  << (float)scanPt->getZValue() << std::endl ;
				
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
	
	pose6d pose (0.0,0.0,0.0,xAngle, yAngle, zAngle); // center

	snode = new ScanNode(pcloud,pose,1);
	
	//tree->insertPointCloud(*snode,-1,true,false);
	mtx_da.lock();
	
	if (scan_nodes.size() > MAX_SCAN_NODES) {
		if ((skip_counter = skip_counter-1) < SKIP_MIN)
			skip_counter = SKIP_MIN;
	}
	else if (scan_nodes.size() < MAX_SCAN_NODES) {
		if ((skip_counter = skip_counter+1) > SKIP_MAX)
			skip_counter = SKIP_MAX;
	}
	
	if (sc == skip_counter) {
		scan_nodes.push_back(*snode);
		sc = 0;
	} else
		sc++;
	
	mtx_da.unlock();
	
	// calc delay
	thread_clock::time_point stop = thread_clock::now();
	if (TRUE) {
		
		std::cout << "Callback: " << duration_cast<milliseconds>(stop - start).count() << " ms\n";
	}
	
	// clean up
	//delete pcloud;
	
	cb_start = thread_clock::now();

	return;
}

// get octomap data for lds server
int CReader::getOctomap(std::stringstream& buff)
{
	// clear stream
	buff.str(std::string());
	mtx_da.lock();
	if (data_av == 1) {
		buff << point_stream.rdbuf();
		data_av = 0;
		mtx_da.unlock();
		return 1;
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
    
    // stop worker thread
    state = THREAD_STOP;
    tworker->join();

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

int CReader::writeLaserData(BOOL new_scan, ibeo::IbeoLaserScanpoint * scanPt)
{
	std::fstream f;
	f.open("ld.txt", std::ios::out | std::ios::app);
	if (f.is_open()) {
		if (new_scan)
			f << "NODE" << " " << "0" << " " << "0" << " " << "0" << " " << xAngle << " " << yAngle << " " << zAngle << "\n";
		else
			f << scanPt->getXValue() << " " << scanPt->getYValue() << " " << scanPt->getZValue() << "\n";
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
	scan_nodes.clear();

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


