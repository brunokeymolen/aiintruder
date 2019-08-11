FTP_STATIC_LIB=ftpserver.a
FTP_CFLAGS=-Wall -g3 -ggdb -O0 -I. -I.. -I ../common 
FTP_LDFLAGS=-no-pie -lpthread
FTP_BINARY=server
CC=g++
CPP_FLAGS=-std=c++11
LIBRARIES=libconfig++ 
#opencv

CPP_FLAGS+=`pkg-config ${LIBRARIES} --cflags`
FTP_LDFLAGS+=`pkg-config ${LIBRARIES} --libs`


FTP_SRC =	ftpserver.o \
			_string.o

FTP_SRC_MAIN = main.o \
			   ../common/options.cpp

FTP_SRC_ALL = $(FTP_SRC) $(FTP_SRC_MAIN)

all: ftpserver-main

ftpserver-main: $(FTP_SRC_ALL)
	g++ $(CPP_FLAGS) $(FTP_SRC_ALL) $(FTP_CFLAGS) $(FTP_LDFLAGS) -o server


ftpserver-static-lib: $(FTP_SRC)
	ar rcs $(FTP_STATIC_LIB) $(FTP_SRC)

%.o: %.c %.h 
	$(CC) $(CPP_FLAGS) $(FTP_CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CPP_FLAGS) $(FTP_CFLAGS) -c -o $@ $<

clean: ftpserver-clean

ftpserver-clean: 
	[ -f server ] && rm  server || true
	[ -f $(FTP_STATIC_LIB) ] && -rm *.a || true
	[ -f ftpserver.o ] && -rm *.o || true
	
