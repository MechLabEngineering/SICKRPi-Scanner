SPATH = /home/markus/Programme/libs/

#quelldateien
SOURCES_IBEO =  main.cpp \
		CReader.cpp \
		ip_connection.cpp \
		brick_imu.cpp \
		$(SPATH)IbeoLux/src/Capture/CaptureToCSV.cpp \
		$(SPATH)IbeoLux/src/Capture/CaptureToFileAbstract.cpp \
	 	$(SPATH)IbeoLux/src/IbeoLaser/IbeoLUX.cpp \
		$(SPATH)IbeoLux/src/IbeoLaser/IbeoLUXData.cpp \
		$(SPATH)IbeoLux/src/IbeoLaser/IbeoLaserAbstract.cpp \
		$(SPATH)IbeoLux/src/IbeoLaser/IbeoLaserDataAbstract.cpp \
		$(SPATH)IbeoLux/src/IbeoLaser/IbeoLaserScanpoint.cpp \
		$(SPATH)IbeoLux/src/Additional/Buffers/Ringbuffer.cpp \
		$(SPATH)IbeoLux/src/Additional/XMath/Convert.cpp \
		$(SPATH)IbeoLux/src/Additional/XMath/Vector.cpp \
		$(SPATH)IbeoLux/src/Additional/XMath/Vector3.cpp \
		$(SPATH)IbeoLux/src/Additional/XMath/Matrix.cpp \
		$(SPATH)IbeoLux/src/Additional/XMath/MovingMatrix.cpp

SOURCES = $(SOURCES_IBEO)

#namen der objektdateien
OBJECTS = $(SOURCES:.cpp=.o) 

INCLUDES = -I $(SPATH) -I $(SPATH)boost_1_56_0 -I $(SPATH)octomap/octomap/include

CXXFLAGS = -g -Wall $(INCLUDES) #-m32

LFLAGS = -L $(SPATH)IbeoLux/build/lib -L $(SPATH)boost_1_56_0/stage/lib -L $(SPATH)octomap/lib

LIBS = -lrt -IbeoLux -lboost_system -lboost_signals -lboost_chrono -lboost_thread -lpthread -loctomap -loctomath -pthread

#name der main-datei und ausfuehrbaren
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ $(CXXFLAGS) $(OBJECTS) $(LFLAGS) $(LIBS) -O3 -o $(TARGET)

clean:
	rm -f $(OBJECTS) 
