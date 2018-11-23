# MyFirstRootkit
Simple kernel mode rootkit for Windows 10 - for an Operating Systems class.

*Files:*

  makefile.mak: makefile for ez building on systems with make installed

  malicious_dll.cpp: source code for the dll with API hooks
      - Should be compiled to system64.dll
  
  payload.cpp: source code for virus payload (just plays a tune)
      - Should be compiled to x64audioservice.exe
  
  lib: various includes (including mhook library for ez hooking)



*Components/functions:*
  
  *dropper:
  
  -gains administrative privileges
    
  -places x64audioservice.exe and system64.dll into a directory together
    
  -starts x64audioservice.exe in silent mode (ie no cmd popup)
    *Note: must be done before loading system64.dll
    
  -sets system64.dll to be loaded via the AppInit registry key
		
  *rootkit (system64.dll):
  
  -hooks NTQuerySystemInformation() and removes x64audioservice.exe from list of processes
  
  -hooks (probably) NTOpenFile() and removes all mentions of the folder containing x64audioservice.exe (and possibly the dll itself)
  
  -sets a shutdown callback that sets x64audioservice.exe to run on start (via registry)
  
  -deletes the onstart registry key for x64audioservice.exe once it starts
  
  -hooks functions to prevent user from controlling their volume through windows
	
  *payload (x64audioservice.exe):
 
  -plays a tune to annoy the user
