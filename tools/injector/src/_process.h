/*
 * @Author: lmintlcx, modified by yuchenxi0_0
 * @Date: 2018-06-27 21:53:24
 * @Description: Read and write memory.
 */

#pragma once
#include <cassert>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <process.h>
#include <sstream>
#include <string>

#include <Windows.h>
#include <tlhelp32.h>

class Process {

protected:
    HWND hwnd;
    DWORD pid;
    HANDLE handle;

public:
    Process();
    ~Process();

    bool OpenByWindow(const wchar_t*, const wchar_t*);
    bool IsValid();

    DWORD InjectDLL(PCWSTR);
    DWORD EjectDLL();
    void ManageDLL();
    void ExamineEnvironment();

    void Write(uintptr_t addr, size_t len, uint8_t* data);

    template <typename T, typename... Args>
    T ReadMemory(Args... args)
    {
        std::initializer_list<uintptr_t> lst = {static_cast<uintptr_t>(args)...};
        uintptr_t buff = 0;
        T result = T();
        for (auto it = lst.begin(); it != lst.end(); ++it)
            if (it != lst.end() - 1)
                ReadProcessMemory(handle, (const void*)(buff + *it), &buff, sizeof(buff), nullptr);
            else
                ReadProcessMemory(handle, (const void*)(buff + *it), &result, sizeof(result), nullptr);
        return result;
    }
};