CC=i686-w64-mingw32.static-g++
CFLAGS=-g -Wall -std=c++11 -static
LIB= -L../../zlib/ -lz
INC=-I ../../zlib/
TARGET=../bin/readhook_test.exe
OUTDIR=

all:
	$(CC) $(CFLAGS) $(INC) $(LIB) ./*.cpp -o $(TARGET)
	echo DONE
