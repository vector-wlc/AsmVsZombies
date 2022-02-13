/*
 * @coding: utf-8
 * @Author: yuchenxi0_0
 * @Date: 2020-02-06 10:22:46
 * @Description: memory
 */

#include "memory.h"

#include <iostream>

Memory::Memory() { handle = nullptr; }

Memory::~Memory()
{
    if (IsValid())
        CloseHandle(handle);
}

bool Memory::OpenSelf()
{
    handle = GetCurrentProcess();
    return true;
}

PVOID Memory::Alloc(DWORD len)
{
    return VirtualAlloc(0, len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

BOOL Memory::Free(PVOID addr) { return VirtualFree(addr, 0, MEM_RELEASE); }

BOOL Memory::Write(uintptr_t addr, size_t len, void* data)
{
    return WriteProcessMemory(handle, (void*)addr, data, len, nullptr);
}

bool Memory::IsValid() { return handle != 0; }
