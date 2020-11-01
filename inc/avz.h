/*
 * *** coding: utf-8
 * *** Author: yuchenxi0_0 and vector-wlc
 * *** Date: 2020-02-06 10:22:46
 * *** Description: High-precision PvZ TAS Frameworks : Assembly vs. Zombies !
 *               The founder is yuchenxi0_0.
 *               The underlying implementation is completed by yuchenxi0_0.
 *               The interface compatible with other frameworks is completed by vector-wlc.
 */

#ifndef __AVZ_H__
#define __AVZ_H__

#include "avz_compatible.h"

void Script();

// The codes written in this function will run every tick
#ifdef __MINGW32__
void RunScriptEveryTick(MainObject *level)
{
#else
void RunScriptEveryTick()
{
    MainObject *level;
    __asm {
		mov level, ebx
    }
#endif

    AvZ::__Run(level, Script);

#ifndef __MINGW32__
    __asm
    {
		mov ebx, level 
		mov eax, 0x4130d0 
		call eax
    }
#endif
}

#ifdef __MINGW32__
uintptr_t addr = 0;
#endif

// call script() instead of game_loop()
void InstallHook(Memory &memory)
{
#ifdef __MINGW32__
    addr = (uintptr_t)memory.Alloc(4096);
    /*
    0x415945:
    call addr
    */
    uint8_t call_addr[5];
    // call addr
    call_addr[0] = 0xe8;
    (int &)call_addr[1] = addr - (0x415945 + 5);
    memory.Write(0x415945, sizeof(call_addr), call_addr);
    /*
    addr:
    push ebx
    call _script
    pop ebx
    call 0x4130d0
    ret
    */
    uint8_t patch[13];
    // push ebx
    patch[0] = 0x53;
    // call _script
    patch[1] = 0xe8;
    (int &)patch[2] = (uintptr_t)RunScriptEveryTick - (addr + 1 + 5);
    // pop ebx
    patch[6] = 0x5b;
    // call 4130d0
    patch[7] = 0xe8;
    (int &)patch[8] = 0x4130d0 - (addr + 7 + 5);
    // ret
    patch[12] = 0xc3;
    memory.Write(addr, sizeof(patch), patch);
#else
    /*
    0x415945:
    call script
    */
    uint8_t call_addr[5];
    // call script
    call_addr[0] = 0xe8;
    (int &)call_addr[1] = (uintptr_t)RunScriptEveryTick - (0x415945 + 5);
    memory.Write(0x415945, sizeof(call_addr), call_addr);
#endif
}

void UninstallHook(Memory &memory)
{
    /*
    0x415945:
    call 0x4130d0
    */
    uint8_t call_addr[5];
    // call addr
    call_addr[0] = 0xe8;
    (int &)call_addr[1] = 0x4130d0 - (0x415945 + 5);
    memory.Write(0x415945, sizeof(call_addr), call_addr);
#ifdef __MINGW32__
    memory.Free((PVOID)addr);
#endif
}

Memory memory;

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // attach to process
        // return FALSE to fail DLL load
        memory.OpenSelf();
        InstallHook(memory);
        break;

    case DLL_PROCESS_DETACH:
        // detach from process
        AvZ::__Exit();
        Sleep(10);

        UninstallHook(memory);
        break;

    case DLL_THREAD_ATTACH:
        // attach to thread
        break;

    case DLL_THREAD_DETACH:
        // detach from thread
        break;
    }
    return TRUE; // succesful
}
#endif