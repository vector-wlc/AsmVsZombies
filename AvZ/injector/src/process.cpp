#include <iostream>
#include <Windows.h>
#include "_process.h"

Process::Process()
{
	hwnd = nullptr;
	pid = 0;
	handle = nullptr;
}

Process::~Process()
{
	if (IsValid())
		CloseHandle(handle);
}

bool Process::OpenByWindow(const wchar_t *class_name, const wchar_t *window_name)
{
	if (IsValid())
		CloseHandle(handle);
	hwnd = FindWindowW(class_name, window_name);
	if (hwnd != nullptr)
		GetWindowThreadProcessId(hwnd, &pid);
	if (pid != 0)
		handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

#ifdef _DEBUG
	std::cout << "OpenByWindow: ";
	std::wcout << (class_name == nullptr ? L"nullptr" : class_name) << L"  "
			   << (window_name == nullptr ? L"nullptr" : window_name);
	std::cout << "  " << hwnd << "  " << pid << "  " << handle << std::endl;
#endif

	return hwnd != nullptr;
}

bool Process::OpenByPid(DWORD pid)
{
	this->pid = pid;
	handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
#ifdef _DEBUG
	if (handle == 0)
	{
		printf("failed to open process\n");
	}
#endif // _DEBUG
	return handle != 0;
}

void Process::ManageDLL()
{
	PCWSTR goal_path_name = L"C:/ProgramData/PopCap Games/PlantsVsZombies/userdata/libpvz.dll";
	EjectDLL();
	if (!CopyFileW(L"libpvz.dll", goal_path_name, false))
	{
		if (!CopyFileW(L"./build/libpvz.dll", goal_path_name, false))
		{
			MessageBoxW(NULL, L"Can't Find libpvz.dll!", L"Error", MB_ICONERROR);
		}
	}
	if (!InjectDLL(goal_path_name))
	{
		MessageBoxW(NULL, L"InjectDLL libpvz.dll failed! Do you open PvZ?", L"Error", MB_ICONERROR);
	}
}

DWORD Process::EjectDLL()
{
	// Copy from Internet =_=
	const char *szDllName = "libpvz.dll";
	BOOL bMore = FALSE, bFound = FALSE;
	HANDLE hSnapshot, hProcess, hThread;
	HMODULE hModule = NULL;
	MODULEENTRY32 me = {sizeof(me)};
	LPTHREAD_START_ROUTINE pThreadProc;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	bMore = Module32First(hSnapshot, &me);
	for (; bMore; bMore = Module32Next(hSnapshot, &me))
	{
		if (!strcmp(me.szModule, szDllName) || !strcmp(me.szExePath, szDllName))
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)))
	{
		wprintf(L"OpenProcess(%d) failed!!! [%d]\n,", pid, GetLastError());
		return FALSE;
	}
	hModule = GetModuleHandle("Kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);
	return TRUE;
}

DWORD Process::InjectDLL(PCWSTR pszLibFile)
{
	// Calculate the number of bytes needed for the DLL's pathname
	DWORD dwSize = (lstrlenW(pszLibFile) + 1) * sizeof(wchar_t);
	if (handle == NULL)
	{
		wprintf(L"[-] Error: Could not open process for PID (%d).\n", pid);
		return FALSE;
	}

	// Allocate space in the remote process for the pathname
	LPVOID pszLibFileRemote = (PWSTR)VirtualAllocEx(handle, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (pszLibFileRemote == NULL)
	{
		wprintf(L"[-] Error: Could not allocate memory inside PID (%d).\n", pid);
		return FALSE;
	}

	// Copy the DLL's pathname to the remote process address space
	DWORD n = WriteProcessMemory(handle, pszLibFileRemote, (PVOID)pszLibFile, dwSize, NULL);
	if (n == 0)
	{
		wprintf(L"[-] Error: Could not write any bytes into the PID [%d] address space.\n", pid);
		return FALSE;
	}

	// Get the real address of LoadLibraryW in Kernel32.dll
	LPTHREAD_START_ROUTINE pfnThreadRtn = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
	if (pfnThreadRtn == NULL)
	{
		wprintf(L"[-] Error: Could not find LoadLibraryA function inside kernel32.dll library.\n");
		return FALSE;
	}

	// Create a remote thread that calls LoadLibraryW(DLLPathname)
	HANDLE hThread = CreateRemoteThread(handle, NULL, 0, pfnThreadRtn, pszLibFileRemote, 0, NULL);
	if (hThread == NULL)
	{
		wprintf(L"[-] Error: Could not create the Remote Thread.\n");
		return FALSE;
	}

	// Wait for the remote thread to terminate
	WaitForSingleObject(hThread, INFINITE);

	// Free the remote memory that contained the DLL's pathname and close Handles
	if (pszLibFileRemote != NULL)
		VirtualFreeEx(handle, pszLibFileRemote, 0, MEM_RELEASE);

	if (hThread != NULL)
		CloseHandle(hThread);

	return TRUE;
}

void Process::Write(uintptr_t addr, size_t len, uint8_t *data)
{
	WriteProcessMemory(handle, (void *)addr, data, len, nullptr);
}

bool Process::IsValid()
{
	if (handle == nullptr)
		return false;

	DWORD exit_code;
	GetExitCodeProcess(handle, &exit_code);
	bool valid = (exit_code == STILL_ACTIVE);

#ifdef _DEBUG
	if (!valid)
		std::cout << "Not Valid" << std::endl;
#endif

	return valid;
}
