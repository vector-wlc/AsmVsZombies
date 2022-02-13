/*
 * @Author: lmintlcx, modified by yuchenxi0_0
 * @Date: 2018-06-27 21:53:24
 * @Description: Read and write memory.
 */

#pragma once

#include <Windows.h>
#include <tlhelp32.h>

#include <array>
#include <cassert>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class Memory {
public:
    Memory();
    ~Memory();

    bool OpenSelf();
    bool IsValid();

    PVOID Alloc(DWORD len);
    BOOL Free(PVOID addr);

    BOOL Write(uintptr_t addr, size_t len, void* data);

    template <typename T>
    T ReadMemory(std::initializer_list<uintptr_t>);
    template <typename T>
    void WriteMemory(T, std::initializer_list<uintptr_t>);

    template <typename T, size_t size>
    std::array<T, size> ReadMemory(std::initializer_list<uintptr_t>);
    template <typename T, size_t size>
    void WriteMemory(std::array<T, size>, std::initializer_list<uintptr_t>);

    template <typename T, typename... Args>
    T ReadMemory(Args...);
    template <typename T, typename... Args>
    void WriteMemory(T, Args...);

    template <typename T, size_t size, typename... Args>
    std::array<T, size> ReadMemory(Args...);
    template <typename T, size_t size, typename... Args>
    void WriteMemory(std::array<T, size>, Args...);

protected:
    HANDLE handle;
};

template <typename T>
T Memory::ReadMemory(std::initializer_list<uintptr_t> addr)
{
    T result = T();

    if (IsValid()) {
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                ReadProcessMemory(handle, (const void*)(offset + *it), &offset, sizeof(offset), nullptr);
            else
                ReadProcessMemory(handle, (const void*)(offset + *it), &result, sizeof(result), nullptr);
#ifdef _DEBUG
        auto int_to_hex_string = [](unsigned int num) -> std::string {
            std::stringstream sstream;
            sstream << "0x" << std::hex << num;
            return sstream.str();
        };

        auto addr2string = [&int_to_hex_string](std::initializer_list<uintptr_t> addr_list) -> std::string {
            std::string str;
            for (auto it = addr_list.begin(); it != addr_list.end(); it++)
                if (it == addr_list.begin())
                    str = "[" + int_to_hex_string(*it) + "]";
                else
                    str = "[" + str + " +" + int_to_hex_string(*it) + "]";
            return str;
        };

        // [[[0x6a9ec0] +0x768] +0x5560] --> 8000 / 0x1f40
        std::cout << addr2string(addr)
                  << " --> "
                  << std::dec << static_cast<unsigned int>(result)
                  << " / "
                  << std::hex << int_to_hex_string(static_cast<unsigned int>(result))
                  << std::endl;
#endif
    }

    return result;
}

template <typename T>
void Memory::WriteMemory(T value, std::initializer_list<uintptr_t> addr)
{
    if (IsValid()) {
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                ReadProcessMemory(handle, (const void*)(offset + *it), &offset, sizeof(offset), nullptr);
            else
                WriteProcessMemory(handle, (void*)(offset + *it), &value, sizeof(value), nullptr);
#ifdef _DEBUG
        std::cout << "Write: ";
        T read = ReadMemory<T>(addr);
        if (read != value)
            std::cout << "Failed: " << int(read) << std::endl;
#endif
    }
}

template <typename T, size_t size>
std::array<T, size> Memory::ReadMemory(std::initializer_list<uintptr_t> addr)
{
    std::array<T, size> result = {T()};

    if (IsValid()) {
        T buff[size] = {0};
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                ReadProcessMemory(handle, (const void*)(offset + *it), &offset, sizeof(offset), nullptr);
            else {
                ReadProcessMemory(handle, (const void*)(offset + *it), &buff, sizeof(buff), nullptr);
#ifdef _DEBUG
                for (size_t i = 0; i < size; i++) {
                    std::cout << "Read Array: ";
                    ReadMemory<T>({offset + *it + i * sizeof(T)});
                }
#endif
            }
        for (size_t i = 0; i < size; i++)
            result[i] = buff[i];
    }

    return result;
}

template <typename T, size_t size>
void Memory::WriteMemory(std::array<T, size> value, std::initializer_list<uintptr_t> addr)
{
    if (IsValid()) {
        T buff[size] = {0};
        for (size_t i = 0; i < size; i++)
            buff[i] = value[i];
        uintptr_t offset = 0;
        for (auto it = addr.begin(); it != addr.end(); it++)
            if (it != addr.end() - 1)
                ReadProcessMemory(handle, (const void*)(offset + *it), &offset, sizeof(offset), nullptr);
            else {
                WriteProcessMemory(handle, (void*)(offset + *it), &buff, sizeof(buff), nullptr);
#ifdef _DEBUG
                for (size_t i = 0; i < size; i++) {
                    std::cout << "Write Array: ";
                    T read = ReadMemory<T>({offset + *it + i * sizeof(T)});
                    if (read != value[i])
                        std::cout << "Failed: " << int(read) << std::endl;
                }
#endif
            }
    }
}

template <typename T, typename... Args>
T Memory::ReadMemory(Args... address)
{
    return ReadMemory<T>({static_cast<uintptr_t>(address)...});
}

template <typename T, typename... Args>
void Memory::WriteMemory(T value, Args... address)
{
    WriteMemory<T>(value, {static_cast<uintptr_t>(address)...});
}

template <typename T, size_t size, typename... Args>
std::array<T, size> Memory::ReadMemory(Args... address)
{
    return ReadMemory<T, size>({static_cast<uintptr_t>(address)...});
}

template <typename T, size_t size, typename... Args>
void Memory::WriteMemory(std::array<T, size> value, Args... address)
{
    WriteMemory<T, size>(value, {static_cast<uintptr_t>(address)...});
}
