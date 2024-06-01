/*
 * @Coding: utf-8
 * @Author: 零度
 * @Date: 2023-01-16 10:06:40
 * @Description:
 */
#ifndef __SEH_H__
#define __SEH_H__

#define _CRT_SECURE_NO_WARNINGS 1
#include "windows.h"
#include <DbgHelp.h>
#include <functional>
#include <map>
#include <string>
class ASeh {
    static LRESULT CALLBACK SEHWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static long __stdcall UnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptPtr);
    static bool GetLogicalAddress(void* addr, char* szModule, DWORD len, DWORD& section, DWORD& offset);
    static const char* GetFilename(const char* thePath);
    static void ShowErrorDialog(const char* theErrorTitle, const char* theErrorText);
    static bool CheckImageHelp();
    static std::string IntelWalk(PCONTEXT theContext, int theSkipCount);
    static std::string ImageHelpWalk(PCONTEXT theContext, int theSkipCount);

public:
    ASeh();
    ~ASeh();
};

class ABreakPoint {
    friend ASeh;
    struct Info {
        unsigned char origin;
        std::function<void(CONTEXT*)> callback;
    };
    static inline std::multimap<unsigned char*, Info> breakpoints{};
    static inline unsigned char* lastaddr = nullptr;
    std::multimap<unsigned char*, Info>::iterator info;

public:
    ABreakPoint(void* addr, std::function<void(CONTEXT*)> callback);
    ~ABreakPoint();
};

inline ASeh __aSeh;
#endif