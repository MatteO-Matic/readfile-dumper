CC=i686-w64-mingw32.static-g++
CFLAGS=-g -Wall -std=c++11
LIB= -L../../zlib/ -lz -L.
INC=-I ../../zlib/ -I../../zipper/zipper/
TARGET=
OUTDIR=

all:
	$(CC) $(LIB) $(INC) -o ../bin/readhook_test.exe ./*.cpp -lZipper-static
