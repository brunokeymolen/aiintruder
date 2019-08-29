# (C)2019, Bruno Keymolen (bruno.keymolen@com)
# http://www.keymolen.com

CXX=g++ 
CC=g++
OPTFLAGS=-g3 -ggdb -O0
CXXFLAGS=-std=c++11 -Wall -I. -Icommon -I/usr/local/include $(OPTFLAGS)
CFLAGS=-Wall $(OPTFLAGS)
LDFLAGS= -L/usr/local/lib -lpthread $(OPTFLAGS) 
FTPSERVER=ftpserver/server
SHELL=/bin/bash
TARGET=aiintruder

#opencv4-no-inference
LIBRARIES=libconfig++ opencv libavformat libavcodec libswscale

#opencv
CXXFLAGS+= `PKG_CONFIG_PATH=/opt/keymolen/lib/pkgconfig pkg-config $(LIBRARIES) --cflags`
LDFLAGS+= `PKG_CONFIG_PATH=/opt/keymolen/lib/pkgconfig pkg-config $(LIBRARIES) --libs`

SRC=ftpserver/ftpserver.o \
		ftpserver/_string.o

SRC+=	main.o \
		common/threadpool.o \
		common/options.o \
		aiintruder.o \
		frame_analyzer.o \
		jdecoder.o \
		vdecoder.o 
	
#all: ftpserver-static-lib vdecoder
all: vdecoder 

vdecoder: $(SRC) $(MODULES)
	$(CXX) $(MODULES) $(SRC) $(LDFLAGS) -o $(TARGET)

#$(FTPSERVER):
#	cd ftpserver && $(MAKE) 

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

	
dependencies:
	sudo apt install libconfig++-dev

yolo:
	@echo "YoloV3 - full"
	wget https://pjreddie.com/media/files/yolov3.weights
	wget https://github.com/pjreddie/darknet/blob/master/cfg/yolov3.cfg?raw=true -O ./yolov3.cfg
	wget https://github.com/pjreddie/darknet/blob/master/data/coco.names?raw=true -O ./coco.names
	@echo "YoloV3 - tiny"
	wget https://github.com/pjreddie/darknet/blob/master/cfg/yolov3-tiny.cfg?raw=true -O ./yolov3-tiny.cfg
	wget https://pjreddie.com/media/files/yolov3-tiny.weights?raw=true -O ./yolov3-tiny.weights
	
clean:
	rm -Rf *.o $(TARGET)
	rm -f ftpserver/*.o
	rm -f common/*.o

make distclean:
	make clean


PREFIX ?= /usr


.PHONY: clean all vdecoder install
