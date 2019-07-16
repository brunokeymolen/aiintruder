# (C)2019, Bruno Keymolen (bruno.keymolen@com)
# http://www.keymolen.com

CXX=g++ 
CC=gcc
OPTFLAGS=-g3 -ggdb -O0
CXXFLAGS=-std=c++11 -Wall -I. -I/usr/local/include $(OPTFLAGS)
CFLAGS=-Wall $(OPTFLAGS)
LDFLAGS= -L/usr/local/lib -lpthread $(OPTFLAGS) 
FTPSERVER=ftpserver/server
ENVIRONMENT=/tmp/cam-00.pipe
SHELL=/bin/bash
TARGET=aiintruder

#opencv
CXXFLAGS+=`pkg-config opencv --cflags`
LDFLAGS+=`pkg-config opencv --libs`

SRC =	main.o \
		frame_analyzer.o
	
#all: ftpserver-static-lib vdecoder
all: vdecoder $(FTPSERVER) $(ENVIRONMENT)

vdecoder: $(SRC) $(MODULES)
	$(CXX) $(MODULES) $(SRC) $(LDFLAGS) -o $(TARGET)

$(FTPSERVER):
	cd ftpserver && $(MAKE) 

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(ENVIRONMENT):
	mkfifo /tmp/cam-0{0,1,2,3,4}.pipe

dependencies:
	@echo "YoloV3 - full"
	wget https://pjreddie.com/media/files/yolov3.weights
	wget https://github.com/pjreddie/darknet/blob/master/cfg/yolov3.cfg?raw=true -O ./yolov3.cfg
	wget https://github.com/pjreddie/darknet/blob/master/data/coco.names?raw=true -O ./coco.names
	@echo "YoloV3 - tiny"
	wget https://github.com/pjreddie/darknet/blob/master/cfg/yolov3-tiny.cfg?raw=true -O ./yolov3-tiny.cfg
	wget https://pjreddie.com/media/files/yolov3-tiny.weights?raw=true -O ./yolov3-tiny.weights

clean:
	rm -Rf *.o $(TARGET)

make distclean:
	rm -f /tmp/cam-0{0,1,2,3,4}.pipe
	make clean
	cd ftpserver && $(MAKE) clean


PREFIX ?= /usr


.PHONY: clean all vdecoder install
