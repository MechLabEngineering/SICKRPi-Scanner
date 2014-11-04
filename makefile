#quelldateien
SOURCES_IBEO =  main.cpp \
		CReader.cpp \
		ip_connection.cpp \
		brick_imu.cpp \
		../IbeoLux/src/Capture/CaptureToCSV.cpp \
		../IbeoLux/src/Capture/CaptureToFileAbstract.cpp \
	 	../IbeoLux/src/IbeoLaser/IbeoLUX.cpp \
		../IbeoLux/src/IbeoLaser/IbeoLUXData.cpp \
		../IbeoLux/src/IbeoLaser/IbeoLaserAbstract.cpp \
		../IbeoLux/src/IbeoLaser/IbeoLaserDataAbstract.cpp \
		../IbeoLux/src/IbeoLaser/IbeoLaserScanpoint.cpp \
		../IbeoLux/src/Additional/Buffers/Ringbuffer.cpp \
		../IbeoLux/src/Additional/XMath/Convert.cpp \
		../IbeoLux/src/Additional/XMath/Vector.cpp \
		../IbeoLux/src/Additional/XMath/Vector3.cpp \
		../IbeoLux/src/Additional/XMath/Matrix.cpp \
		../IbeoLux/src/Additional/XMath/MovingMatrix.cpp

SOURCES = $(SOURCES_IBEO)

SPATH = /home/gus484/Programme/

#namen der objektdateien
OBJECTS = $(SOURCES:.cpp=.o) 

INCLUDES = -I ../ -I ../boost_1_56_0 -I $(SPATH)octomap/include

CXXFLAGS = -g -Wall $(INCLUDES)

LFLAGS = -L $(SPATH)IbeoLux/build/lib -L $(SPATH)boost_1_56_0/stage/lib -L $(SPATH)octomap/lib

LIBS = -lrt -IbeoLux -lboost_system -lboost_signals -lboost_chrono -lboost_thread -lpthread -loctomap -loctomath -pthread

#name der main-datei und ausfuehrbaren
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ $(CXXFLAGS) $(OBJECTS) $(LFLAGS) $(LIBS) -O3 -o $(TARGET)

clean:
	rm -f $(OBJECTS) 
