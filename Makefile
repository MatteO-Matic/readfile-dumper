CC=i686-w64-mingw32.static-g++
CFLAGS=-g -Wall -shared -std=c++11 -D_WIN32_WINNT=0x600
LIB=-L../minhook/static/lib
INC=-I../minhook/static/include/ -I../minhook/src/hde
TARGET=readhook.dll
OUTDIR=bin/

all:
	if [ ! -d "bin" ]; then \
		mkdir bin; \
	fi

	$(CC) $(CFLAGS) -o $(OUTDIR)$(TARGET) $(INC) $(LIB) ./*.cpp ../minhook/src/*.c ../minhook/src/hde/*.c
	make -C ./readfile_test
