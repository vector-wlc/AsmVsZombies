#pragma once

#include <Windows.h>
#include <initializer_list>
#include <string>
#include <vector>

class Process {
public:
    template <typename T, typename... Args>
    static T ReadMemory(HANDLE handle, Args... args) {
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

protected:
    std::vector<HWND> _hwnds;
    HWND _selHwnd = nullptr;
    DWORD _pid;
    HANDLE _handle;
    static BOOL CALLBACK _EnumWindowsProc(HWND hwnd, LPARAM lParam);
    void _GetPvzHwnd();
    void _SelectPvzHwnd();
    static void _DealPvzWindow(HWND hwnd);
    void _RemoveAllInjectedDll();

public:
    Process();
    ~Process();

    bool SelectWindow();
    bool IsValid();
    DWORD InjectDLL(PCWSTR);
    DWORD EjectDLL(const std::string& dllName);
    void ManageDLL();
    void Write(uintptr_t addr, size_t len, uint8_t* data);
};