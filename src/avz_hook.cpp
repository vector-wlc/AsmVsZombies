#include "libavz.h"

extern "C" __declspec(dllexport) void __cdecl __AScriptHook() {
    __aScriptManager.ScriptHook();
}

void __AInstallHook() {
    DWORD temp;
    VirtualProtect((void*)0x400000, 0x35E000, PAGE_EXECUTE_READWRITE, &temp);
    *(uint32_t*)0x667bc0 = (uint32_t)&__AScriptHook;
}

void __AUninstallHook() {
    DWORD temp;
    VirtualProtect((void*)0x400000, 0x35E000, PAGE_EXECUTE_READWRITE, &temp);
    *(uint32_t*)0x667bc0 = 0x452650;
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        // attach to process
        // return FALSE to fail DLL load
        __aig.hInstance = hinstDLL;
        __AInstallHook();
        break;

    case DLL_PROCESS_DETACH:
        // detach from process
        __aScriptManager.willBeExit = true;
        for (int i = 0; !__aScriptManager.isExit && i < 50; ++i)
            Sleep(20);
        __AUninstallHook();
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
