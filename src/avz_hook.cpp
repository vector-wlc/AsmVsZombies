/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-09-27 20:16:02
 * @Description:
 */

#include "avz_global.h"
#include "pvzfunc.h"
#include <windows.h>

extern "C" __declspec(dllexport) void __cdecl ManageScript();

// call script() instead of game_loop()
void InstallHook()
{
    DWORD temp;
    VirtualProtect((void*)0x400000, 0x35E000, PAGE_EXECUTE_READWRITE, &temp);
    *(uint32_t*)0x667bc0 = (uint32_t)&ManageScript;
}

void UninstallHook()
{
    DWORD temp;
    VirtualProtect((void*)0x400000, 0x35E000, PAGE_EXECUTE_READWRITE, &temp);
    *(uint32_t*)0x667bc0 = 0x452650;
}