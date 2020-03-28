/*
 * @Author: lmintlcx, modified by yuchenxi0_0
 * @Date: 2018-06-27 21:53:24
 * @Description: Read and write memory.
 */

#pragma once
#include <process.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <functional>
#include <initializer_list>
#include <cassert>

#include <Windows.h>
#include <tlhelp32.h>

class Process
{
public:
    Process();
    ~Process();

    bool OpenByWindow(const wchar_t *, const wchar_t *);
    bool OpenByPid(DWORD pid);
    bool IsValid();

    DWORD InjectDLL(PCWSTR);
    DWORD EjectDLL();
    void ManageDLL();

    void Write(uintptr_t addr, size_t len, uint8_t *data);

protected:
    HWND hwnd;
    DWORD pid;
    HANDLE handle;
};