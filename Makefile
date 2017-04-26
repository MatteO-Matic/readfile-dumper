all:
	i686-w64-mingw32-g++ -shared -o readhook.dll -I ./ ./*.cpp
