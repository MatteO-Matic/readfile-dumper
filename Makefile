all:
	if [ ! -d "bin" ]; then \
		mkdir bin; \
	fi
	i686-w64-mingw32-g++ --static -shared -o bin/readhook.dll -I ./ ./*.cpp
	i686-w64-mingw32-g++ --static -o bin/readhook_test.exe -I ./readfile_test ./readfile_test/*.cpp
