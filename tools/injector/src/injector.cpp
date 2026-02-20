#include "injector.h"

#include <TlHelp32.h>
#include <Windows.h>
#include <array>
#include <direct.h>
#include <filesystem>
#include <iostream>
#include <cstdio>
#include <string>

namespace fs = std::filesystem;

Process::Process() {
    _selHwnd = nullptr;
    _pid = 0;
    _handle = nullptr;
}

Process::~Process() {
    if (IsValid())
        CloseHandle(_handle);
}

BOOL CALLBACK Process::_EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    Process* process = (Process*)lParam;
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == 0)
        return TRUE;
    auto handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

    if (!IsWindowVisible(hwnd))
        return TRUE;

    std::array<WCHAR, 1024> windowText;
    GetWindowTextW(hwnd, windowText.data(), (int)windowText.size());
    std::wstring title(windowText.data());
    if (title != L"Plants vs. Zombies")
        return TRUE;

    // 普通植物血量为 300, 南瓜头血量为 3000
    if (ReadMemory<int>(handle, 0x45DC55) == 300 && ReadMemory<int>(handle, 0x45E445) == 4000)
        process->_hwnds.push_back(hwnd);
    CloseHandle(handle);
    return TRUE;
}

void Process::_GetPvzHwnd() {
    EnumWindows(_EnumWindowsProc, (LPARAM)this);
}

void Process::_DealPvzWindow(HWND hwnd) {
    HWND hForeWnd = ::GetForegroundWindow();
    DWORD dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
    DWORD dwCurID = ::GetCurrentThreadId();
    AttachThreadInput(dwCurID, dwForeID, TRUE);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetForegroundWindow(hwnd);
    AttachThreadInput(dwCurID, dwForeID, FALSE);
}

void Process::_SelectPvzHwnd() {
    _selHwnd = nullptr;
    if (_hwnds.empty()) {
        wprintf(L"请打开 PvZ 或者不要把 PvZ 窗口最小化\n");
        return;
    }
    if (_hwnds.size() == 1) {
        _selHwnd = _hwnds[0];
        DWORD pid;
        GetWindowThreadProcessId(_selHwnd, &pid);
        wprintf(L"注入的窗口为 %d\n", pid);
        return;
    }
    POINT point;
    wprintf(L"鼠标右键双击选择一个 PvZ 窗口以注入\n");
    for (; _selHwnd == nullptr;) {
        Sleep(50);
        GetCursorPos(&point);
        int minDis = INT_MAX;
        // 这个是离鼠标最近的 hwnd
        HWND targetHwnd = nullptr;
        for (auto hwnd : _hwnds) {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            if (rect.top < point.y && rect.bottom > point.y
                && rect.left < point.x && rect.right > point.x) {
                int disX = (rect.left + rect.right) / 2 - point.x;
                int disY = (rect.top + rect.bottom) / 2 - point.y;
                int dis = std::sqrt(disX * disX + disY * disY);
                if (dis < minDis) {
                    minDis = dis;
                    targetHwnd = hwnd;
                }
            }
        }

        if (targetHwnd == nullptr)
            continue;
        _DealPvzWindow(targetHwnd);
        if ((GetAsyncKeyState(VK_RBUTTON) & 0x8001) == 0x8001) {
            _selHwnd = targetHwnd;
            wprintf(L"注入的窗口为 %d\n", _selHwnd);
        }
    }
}

bool Process::SelectWindow() {
    if (IsValid())
        CloseHandle(_handle);
    _GetPvzHwnd();
    _SelectPvzHwnd();
    while (_selHwnd == nullptr) {
        auto ret = MessageBoxW(NULL, L"您是否未打开游戏或者将游戏窗口最小化了? (注意必须是英文原版，steam 版也是不可以的！) \n 点击确定继续尝试注入，点击取消关闭注入", L"Warning", MB_ICONWARNING | MB_OK | MB_OKCANCEL);
        if (ret == 2)
            return false;
        _GetPvzHwnd();
        _SelectPvzHwnd();
    }

    GetWindowThreadProcessId(_selHwnd, &_pid);
    if (_pid != 0)
        _handle = OpenProcess(PROCESS_ALL_ACCESS, false, _pid);

    if (ReadMemory<uint32_t>(_handle, 0x4140c5) != 0x0019b337) {
        MessageBoxW(NULL, L"您使用的游戏版本不是英文原版，请到下载安装包的链接下载 本框架 所支持的英文原版", L"Error", MB_ICONERROR);
        return false;
    }

    auto address = ReadMemory<uintptr_t>(_handle, 0x6a9ec0);
    auto gameUi = ReadMemory<int>(_handle, address + 0x7fc);
    while (gameUi == 2 || gameUi == 3) {
        MessageBoxW(NULL, L"检测到游戏窗口在选卡或战斗界面，这种行为可能会导致注入失败，请在游戏主界面进行注入", L"Warning", MB_ICONWARNING);
        gameUi = ReadMemory<int>(_handle, address + 0x7fc);
    }

    return _selHwnd != nullptr;
}

void Process::_RemoveAllInjectedDll() {
    for (auto&& file : fs::directory_iterator("./bin")) {
        auto fileName = file.path().generic_wstring();
        auto pos = fileName.find(L"libavz_inject");
        if (pos == fileName.npos)
            continue;
        pos += 13 + 1;
        auto len = fileName.size() - 4 - pos;
        if (len <= 0 || pos >= fileName.size()
            || fileName[pos] > '9' || fileName[pos] < '0')
            continue;
        auto pid = std::stoi(fileName.substr(pos, len));
        bool isDelete = true;
        for (auto hwnd : _hwnds) {
            DWORD pidTmp;
            GetWindowThreadProcessId(hwnd, &pidTmp);
            if (pid == pidTmp) {
                isDelete = false;
                break;
            }
        }
        if (!isDelete)
            continue;
        std::error_code ec;
        fs::remove(file, ec);
        if (!ec)
            continue;
        auto handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
        TerminateProcess(handle, 0);
        WaitForSingleObject(handle, 1e4);
        CloseHandle(handle);
        fs::remove(file, ec);
        if (ec)
            wprintf(L"有后台 PvZ 进程，请使用任务管理器杀死 PvZ 进程: %d\n", pid);
    }
}

void Process::ManageDLL(const std::string& dllPath) {
    DWORD pid;
    GetWindowThreadProcessId(_selHwnd, &pid);
    auto libavzInjectPath = fs::path(L"bin/libavz_inject_" + std::to_wstring((uintptr_t)pid) + L".dll");
    EjectDLL(libavzInjectPath.string());
    _RemoveAllInjectedDll();
    libavzInjectPath = fs::absolute(libavzInjectPath);
    auto libavzPath = fs::path(dllPath);
    if (!fs::exists(libavzPath)) {
        MessageBoxW(NULL, L"未检测到 libavz.dll, 请检查您编写的脚本是否有语法错误？", L"Error", MB_ICONERROR);
        return;
    }
    fs::copy(libavzPath, libavzInjectPath, fs::copy_options::overwrite_existing);

    if (!InjectDLL(libavzInjectPath.c_str())) {
        MessageBoxW(NULL, L"libavz.dll 注入失败，失败可能原因如下\n 1. 计算机开启了杀软，此行为被杀软拦截 \n 2. 脚本有语法错误，编译器无法生成动态库文件 \n 3. 本框架 项目路径需要管理员权限才可以进行文件的生成和复制", L"Error", MB_ICONERROR);
        return;
    }

    wprintf(L"AvZ 注入成功，请到选卡界面或者战斗界面查看脚本的运行效果\n");
}

DWORD Process::EjectDLL(const std::string& dllPath) {
    auto pos = dllPath.find_last_of("/");
    auto dllName = pos == dllPath.npos ? dllPath : dllPath.substr(pos + 1, dllPath.npos);
    const char* szDllName = dllName.c_str();
    BOOL bMore = FALSE, bFound = FALSE;
    HANDLE hSnapshot, hProcess, hThread;
    HMODULE hModule = NULL;
    MODULEENTRY32 me = {sizeof(me)};
    LPTHREAD_START_ROUTINE pThreadProc;
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _pid);
    bMore = Module32First(hSnapshot, &me);
    for (; bMore; bMore = Module32Next(hSnapshot, &me)) {
        if (!strcmp(me.szModule, szDllName) || !strcmp(me.szExePath, szDllName)) {
            bFound = TRUE;
            break;
        }
    }
    if (!bFound) {
        CloseHandle(hSnapshot);
        return FALSE;
    }
    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _pid))) {
        wprintf(L"OpenProcess(%d) failed!!! [%d]\n,", _pid, GetLastError());
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

DWORD Process::InjectDLL(PCWSTR pszLibFile) {
    // Calculate the number of bytes needed for the DLL's pathname
    DWORD dwSize = (lstrlenW(pszLibFile) + 1) * sizeof(wchar_t);
    if (_handle == NULL) {
        wprintf(L"[-] Error: Could not open process for PID (%d).\n", _pid);
        return FALSE;
    }

    // Allocate space in the remote process for the pathname
    LPVOID pszLibFileRemote = (PWSTR)VirtualAllocEx(_handle, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
    if (pszLibFileRemote == NULL) {
        wprintf(L"[-] Error: Could not allocate memory inside PID (%d).\n", _pid);
        return FALSE;
    }

    // Copy the DLL's pathname to the remote process address space
    DWORD n = WriteProcessMemory(_handle, pszLibFileRemote, (PVOID)pszLibFile, dwSize, NULL);
    if (n == 0) {
        wprintf(L"[-] Error: Could not write any bytes into the PID [%d] address space.\n", _pid);
        return FALSE;
    }

    // Create a remote thread that calls LoadLibraryW(DLLPathname)
    HANDLE hThread = CreateRemoteThread(_handle, NULL, 0, LPTHREAD_START_ROUTINE(LoadLibraryW), pszLibFileRemote, 0, NULL);
    if (hThread == NULL) {
        wprintf(L"[-] Error: Could not create the Remote Thread.\n");
        return FALSE;
    }

    // Wait for the remote thread to terminate
    WaitForSingleObject(hThread, INFINITE);

    // Free the remote memory that contained the DLL's pathname and close Handles
    if (pszLibFileRemote != NULL)
        VirtualFreeEx(_handle, pszLibFileRemote, 0, MEM_RELEASE);

    if (hThread != NULL)
        CloseHandle(hThread);

    return TRUE;
}

void Process::Write(uintptr_t addr, size_t len, uint8_t* data) {
    WriteProcessMemory(_handle, (void*)addr, data, len, nullptr);
}

bool Process::IsValid() {
    if (_handle == nullptr)
        return false;
    DWORD exit_code;
    GetExitCodeProcess(_handle, &exit_code);
    bool valid = (exit_code == STILL_ACTIVE);

#ifdef _DEBUG
    if (!valid)
        std::cout << "Not Valid" << std::endl;
#endif

    return valid;
}
