#include "avz_seh.h"
#include <fstream>

HINSTANCE hInstance;
HWND buttonClose;
HWND buttonCopy;
std::string errorText;
bool exited;
LPTOP_LEVEL_EXCEPTION_FILTER previousFilter;
char globalBuffer[2048];
struct ErrorType {
    DWORD dwExceptionCode;
    const char* szMessage;
};
std::initializer_list<ErrorType> msgTable {
    {STATUS_SEGMENT_NOTIFICATION, "Segment Notification"},
    {STATUS_BREAKPOINT, "Breakpoint"},
    {STATUS_SINGLE_STEP, "Single step"},
    {STATUS_WAIT_0, "Wait 0"},
    {STATUS_ABANDONED_WAIT_0, "Abandoned Wait 0"},
    {STATUS_USER_APC, "User APC"},
    {STATUS_TIMEOUT, "Timeout"},
    {STATUS_PENDING, "Pending"},
    {STATUS_GUARD_PAGE_VIOLATION, "Guard Page Violation"},
    {STATUS_DATATYPE_MISALIGNMENT, "Data Type Misalignment"},
    {STATUS_ACCESS_VIOLATION, "Access Violation"},
    {STATUS_IN_PAGE_ERROR, "In Page Error"},
    {STATUS_NO_MEMORY, "No Memory"},
    {STATUS_ILLEGAL_INSTRUCTION, "Illegal Instruction"},
    {STATUS_NONCONTINUABLE_EXCEPTION, "Noncontinuable Exception"},
    {STATUS_INVALID_DISPOSITION, "Invalid Disposition"},
    {STATUS_ARRAY_BOUNDS_EXCEEDED, "Array Bounds Exceeded"},
    {STATUS_FLOAT_DENORMAL_OPERAND, "Float Denormal Operand"},
    {STATUS_FLOAT_DIVIDE_BY_ZERO, "Divide by Zero"},
    {STATUS_FLOAT_INEXACT_RESULT, "Float Inexact Result"},
    {STATUS_FLOAT_INVALID_OPERATION, "Float Invalid Operation"},
    {STATUS_FLOAT_OVERFLOW, "Float Overflow"},
    {STATUS_FLOAT_STACK_CHECK, "Float Stack Check"},
    {STATUS_FLOAT_UNDERFLOW, "Float Underflow"},
    {STATUS_INTEGER_DIVIDE_BY_ZERO, "Integer Divide by Zero"},
    {STATUS_INTEGER_OVERFLOW, "Integer Overflow"},
    {STATUS_PRIVILEGED_INSTRUCTION, "Privileged Instruction"},
    {STATUS_STACK_OVERFLOW, "Stack Overflow"},
    {STATUS_CONTROL_C_EXIT, "Ctrl+C Exit"}};

ASeh::ASeh()
{
    previousFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);
    hInstance = GetModuleHandleA(nullptr);
}

ASeh::~ASeh()
{
    SetUnhandledExceptionFilter(previousFilter);
}

long __stdcall ASeh::UnhandledExceptionFilter(LPEXCEPTION_POINTERS lpExceptPtr)
{
    // #ifndef __MINGW32__
    //     __asm__ __volatile__(
    //         "pushal;"
    //         "movl $0x6A9EC0, %%eax;"
    //         "movl (%%eax), %%ecx;"
    //         "testl %%eax, %%ecx;"
    //         "jzl end;"
    //         "movl (%%ecx), %%eax;"
    //         "movl 0x9c(%%eax), %%eax;"
    //         "calll *%%eax;"
    //         "end :"
    //         "popal;");
    // #else
    //     // __asm {
    //     // 	mov eax, 0x6a9ec0
    //     // 	mov ecx, [eax]
    //     // 	test ecx, ecx
    //     // 	jz end
    //     // 	mov eax, [ecx]
    //     // 	mov eax, [eax + 0x9c]
    //     // 	call eax
    //     // 	end :
    //     // }
    // #endif

    DoHandleDebugEvent(lpExceptPtr);
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
    return EXCEPTION_CONTINUE_SEARCH;
}

bool ASeh::CheckImageHelp()
{
    SymSetOptions(SYMOPT_DEFERRED_LOADS);
    GetModuleFileNameA(0, globalBuffer, 2048);
    char* lastdir = strrchr(globalBuffer, '/');
    if (!lastdir)
        lastdir = strrchr(globalBuffer, '\\');
    if (lastdir)
        lastdir[0] = '\0';
    return SymInitialize(GetCurrentProcess(), globalBuffer[0] ? globalBuffer : nullptr, true);
}

void ASeh::DoHandleDebugEvent(LPEXCEPTION_POINTERS lpEP)
{
    bool hasImageHelp = CheckImageHelp();
    std::string errorTitle;
    errorTitle.reserve(2048);
    const char* szName = "Unknown";
    for (auto&& p : msgTable)
        if (p.dwExceptionCode == lpEP->ExceptionRecord->ExceptionCode)
            szName = p.szMessage;
    sprintf(globalBuffer, "Exception: %s (code 0x%lx) at address %p in thread %lx\r\n",
        szName, lpEP->ExceptionRecord->ExceptionCode, lpEP->ExceptionRecord->ExceptionAddress, GetCurrentThreadId());
    errorTitle += globalBuffer;
    DWORD section, offset;
    GetLogicalAddress(lpEP->ExceptionRecord->ExceptionAddress, globalBuffer, 2048, section, offset);
    errorTitle += "Module: ";
    errorTitle += GetFilename(globalBuffer);
    errorTitle += "\r\n";
    sprintf(globalBuffer,
        "Logical Address: %04lX:%08lX\r\nEAX:%08lX    ECX:%08lX    EDX:%08lX\r\nEBX:%08lX    ESI:%08lX    EDI:%08lX\r\nEIP:%08lX    ESP:%08lX    EBP:%08lX\r\nCS: %04lX        SS: %04lX        DS: %04lX\r\nES: %04lX        FS: %04lX        GS: %04lX\r\nFlags:%08lX\r\n",
        section, offset,
        lpEP->ContextRecord->Eax, lpEP->ContextRecord->Ecx, lpEP->ContextRecord->Edx,
        lpEP->ContextRecord->Ebx, lpEP->ContextRecord->Esi, lpEP->ContextRecord->Edi,
        lpEP->ContextRecord->Eip, lpEP->ContextRecord->Esp, lpEP->ContextRecord->Ebp,
        lpEP->ContextRecord->SegCs, lpEP->ContextRecord->SegSs, lpEP->ContextRecord->SegDs,
        lpEP->ContextRecord->SegEs, lpEP->ContextRecord->SegFs, lpEP->ContextRecord->SegGs,
        lpEP->ContextRecord->EFlags);
    errorTitle += globalBuffer;
    std::string walkString;
    if (hasImageHelp)
        walkString = ImageHelpWalk(lpEP->ContextRecord, 0);
    if (walkString.length() == 0)
        walkString = IntelWalk(lpEP->ContextRecord, 0);
    errorText = walkString + errorTitle;
    std::ofstream("crash.txt") << errorText << std::endl;
    ShowErrorDialog(errorTitle.data(), errorText.data());
}

std::string ASeh::IntelWalk(PCONTEXT theContext, int theSkipCount)
{
    std::string dump;
    DWORD pc = theContext->Eip;
    PDWORD pFrame, pPrevFrame;
    pFrame = (PDWORD)theContext->Ebp;
    while (true) {
        char szModule[MAX_PATH] = "";
        DWORD section = 0, offset = 0;
        GetLogicalAddress((PVOID)pc, szModule, sizeof(szModule), section, offset);
        sprintf(globalBuffer, "%08lX  %p  %04lX:%08lX  %s\r\n",
            pc, pFrame, section, offset, GetFilename(szModule));
        dump += globalBuffer;
        pc = pFrame[1];
        pPrevFrame = pFrame;
        pFrame = (PDWORD)pFrame[0];
        if ((DWORD)pFrame & 3 || pFrame <= pPrevFrame || IsBadWritePtr(pFrame, sizeof(PVOID) * 2))
            break;
    };
    return dump;
}

std::string ASeh::ImageHelpWalk(PCONTEXT theContext, int theSkipCount)
{
    std::string dump;
    STACKFRAME sf;
    memset(&sf, 0, sizeof(sf));
    sf.AddrPC.Offset = theContext->Eip;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Offset = theContext->Esp;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = theContext->Ebp;
    sf.AddrFrame.Mode = AddrModeFlat;
    // int level = 0;
    while (true) {
        if (!StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &sf, theContext, NULL, SymFunctionTableAccess, SymGetModuleBase, 0)) {
            if (DWORD lastErr = GetLastError()) {
                sprintf(globalBuffer, "StackWalk failed (error %lu)\r\n", lastErr);
                dump += globalBuffer;
            }
            break;
        }
        if ((sf.AddrFrame.Offset == 0) || (sf.AddrPC.Offset == 0))
            break;
        if (theSkipCount > 0) {
            theSkipCount--;
            continue;
        }
        BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 512];
        PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(symbolBuffer);
        pSymbol->MaxNameLength = 512;
        DWORD symDisplacement = 0;
        int len = 0;
        if (SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &symDisplacement, pSymbol)) {
            char UDName[256];
            UnDecorateSymbolName(pSymbol->Name, UDName, 256, UNDNAME_NO_ALLOCATION_MODEL | UNDNAME_NO_ALLOCATION_LANGUAGE | UNDNAME_NO_MS_THISTYPE | UNDNAME_NO_ACCESS_SPECIFIERS | UNDNAME_NO_THISTYPE | UNDNAME_NO_MEMBER_TYPE | UNDNAME_NO_RETURN_UDT_MODEL | UNDNAME_NO_THROW_SIGNATURES | UNDNAME_NO_SPECIAL_SYMS);
            len = sprintf(globalBuffer, "%08lX  %08lX  %hs+%lX",
                sf.AddrFrame.Offset, sf.AddrPC.Offset, UDName, symDisplacement);
        } else {
            char szModule[MAX_PATH];
            DWORD section = 0, offset = 0;
            GetLogicalAddress((PVOID)sf.AddrPC.Offset, szModule, sizeof(szModule), section, offset);
            len = sprintf(globalBuffer, "%08lX  %08lX  %s:%04lX:%08lX",
                sf.AddrFrame.Offset, sf.AddrPC.Offset, GetFilename(szModule), section, offset);
        }
        dump += globalBuffer;
        if (len > 80)
            len = 0, dump += "\r\n";
        dump.append(80 - len, ' ');
        sprintf(globalBuffer, "Params: %08lX %08lX %08lX %08lX\r\n", sf.Params[0], sf.Params[1], sf.Params[2], sf.Params[3]);
        dump += globalBuffer;
        // level++;
    }
    return dump;
}

bool ASeh::GetLogicalAddress(void* addr, char* szModule, DWORD len, DWORD& section, DWORD& offset)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
        return false;
    DWORD hMod = (DWORD)mbi.AllocationBase;
    if (!GetModuleFileNameA((HMODULE)hMod, szModule, len))
        return false;
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHdr);
    DWORD rva = (DWORD)addr - hMod;
    for (unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++, pSection++) {
        DWORD sectionStart = pSection->VirtualAddress;
#undef max
        DWORD sectionEnd = sectionStart + std::max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);
        if ((rva >= sectionStart) && (rva <= sectionEnd)) {
            section = i + 1;
            offset = rva - sectionStart;
            return true;
        }
    }
    return false;
}

const char* ASeh::GetFilename(const char* thePath)
{
    const char* ans = strrchr(thePath, '/');
    if (!ans)
        ans = strrchr(thePath, '\\');
    if (!ans)
        ans = thePath - 1;
    return ans + 1;
}

LRESULT CALLBACK ASeh::SEHWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_COMMAND:
        if ((HWND)lParam == buttonClose)
            exited = true;
        else if ((HWND)lParam == buttonCopy)
            if (OpenClipboard(0)) {
                EmptyClipboard();
                HANDLE hStr = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(char) * (errorText.size() + 1));
                if (hStr) {
                    char* p = (char*)GlobalLock(hStr);
                    if (p) {
                        strcpy_s(p, errorText.size() + 1, errorText.data());
                        p[errorText.size()] = '\0';
                    }
                    GlobalUnlock(hStr);
                    SetClipboardData(CF_TEXT, hStr);
                }
                CloseClipboard();
            }
        break;
    case WM_CLOSE:
        exited = true;
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void ASeh::ShowErrorDialog(const char* theErrorTitle, const char* theErrorText)
{
    HFONT font = ::CreateFontA(-MulDiv(8, 96, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");
    ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
    WNDCLASSA wc;
    wc.style = 0;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = ::GetSysColorBrush(COLOR_BTNFACE);
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = ::LoadIcon(NULL, IDI_ERROR);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = SEHWindowProc;
    wc.lpszClassName = "SEHWindow";
    wc.lpszMenuName = NULL;
    RegisterClassA(&wc);
    RECT wndRect;
    wndRect.left = 100;
    wndRect.top = 100;
    wndRect.right = 900;
    wndRect.bottom = 700;
    // BOOL worked = AdjustWindowRect(&wndRect, WS_CLIPCHILDREN | WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    int width = wndRect.right - wndRect.left, height = wndRect.bottom - wndRect.top;
    HWND HWnd = CreateWindowA("SEHWindow", "Error!",
        WS_CLIPCHILDREN | WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        wndRect.left,
        wndRect.top,
        width + 8,
        height + 30,
        0, 0, hInstance, 0);
    SendMessage(CreateWindowA("EDIT", theErrorTitle, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY,
                    8, 8, width - 16, 120, HWnd, 0, hInstance, 0),
        WM_SETFONT, (WPARAM)font, 0);
    SendMessage(CreateWindowA("EDIT", theErrorText, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER | WS_VSCROLL | ES_READONLY,
                    8, 136, width - 16, height - 176, HWnd, 0, hInstance, 0),
        WM_SETFONT, (WPARAM)font, 0);
    constexpr int buttonNum = 2;
    int buttonWidth = (width - (buttonNum + 1) * 8) / buttonNum;
    SendMessage(buttonClose = CreateWindowA("BUTTON", "Close", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_PUSHBUTTON,
                    8, height - 32, buttonWidth, 24, HWnd, 0, hInstance, 0),
        WM_SETFONT, (WPARAM)font, 0);
    SendMessage(buttonCopy = CreateWindowA("BUTTON", "Copy", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_PUSHBUTTON,
                    16 + buttonWidth, height - 32, buttonWidth, 24, HWnd, 0, hInstance, 0),
        WM_SETFONT, (WPARAM)font, 0);
    ShowWindow(HWnd, SW_NORMAL);
    MSG msg;
    while ((GetMessage(&msg, NULL, 0, 0) > 0) && (!exited)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DestroyWindow(HWnd);
    DeleteObject(font);
}