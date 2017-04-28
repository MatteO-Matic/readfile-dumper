all:
	if [ ! -d "bin" ]; then \
		mkdir bin; \
	fi
	i686-w64-mingw32-g++ --static -shared -o bin/readhook.dll -L../minhook/static/lib -I../minhook/src/hde -I./ -I../minhook/static/include/ ./*.cpp ../minhook/src/*.c ../minhook/src/hde/*.c
	i686-w64-mingw32-g++ --static -o bin/readhook_test.exe -I ./readfile_test ./readfile_test/*.cpp
