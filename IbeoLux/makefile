BUILD_DIR = ./build
SRC_DIR = ./src
DEPENDFILE = $(BUILD_DIR)/.depend
CXX = g++

SOURCES_ADDITIONAL = 	Additional/Buffers/Ringbuffer.cpp

SOURCES_IBEOLASER =	IbeoLaser/IbeoLaserAbstract.cpp \
			IbeoLaser/IbeoLaserScanpoint.cpp \
			IbeoLaser/IbeoLUXData.cpp \
			IbeoLaser/IbeoLaserDataAbstract.cpp \
			IbeoLaser/IbeoLUX.cpp

SOURCES_CAPTURE	=	Capture/CaptureInterface.cpp \
			Capture/CaptureToFileAbstract.cpp \
			Capture/CaptureToScreen.cpp \
			Capture/CaptureToCSV.cpp \
			Capture/CaptureToScreenAbstract.cpp

SOURCES =		 $(SOURCES_CAPTURE) $(SOURCES_IBEOLASER) $(SOURCES_ADDITIONAL)	

INCLUDES =		-I/home/gus484/Programme/boost_1_56_0

TMP_SRC = $(addprefix $(SRC_DIR)/, $(SOURCES))
TMP_OBJ = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

all: 	$(BUILD_DIR) $(TMP_OBJ)
	@echo "********************************************"
	@echo "Kompilieren"
	@echo "********************************************"
	echo $(TMP_OBJ)
	ar rvs $(BUILD_DIR)/lib/libIbeoLux.a $(TMP_OBJ)

clean:
	rm -rf $(BUILD_DIR)

dep:	$(BUILD_DIR)
	$(CXX) $(INCLUDES) -MM `find ./src -name '*.cpp'` > $(BUILD_DIR)/$(DEPENDFILE)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(INCLUDES) -Wall -c -O3 -o $@ $<


# Verzeichnisse erstellen
$(BUILD_DIR):
	@echo "********************************************"
	@echo "Erstelle Verzeichnisstruktur"
	@echo "********************************************"
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/Additional
	mkdir -p $(BUILD_DIR)/Additional/Buffers
	mkdir -p $(BUILD_DIR)/IbeoLaser
	mkdir -p $(BUILD_DIR)/Capture
	mkdir -p $(BUILD_DIR)/lib
	@echo .
