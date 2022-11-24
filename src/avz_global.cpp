/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-11-09 10:49:00
 * @Description:
 */

#include "avz_global.h"
#include "avz_logger.h"
#include <codecvt>
#include <locale>

__AInternalGlobal __aInternalGlobal;

bool ARangeIn(int num, std::initializer_list<int> lst)
{
    for (auto _num : lst) {
        if (_num == num) {
            return true;
        }
    }
    return false;
}

std::wstring AStrToWstr(const std::string& input)
{
    auto cStr = input.c_str();
    auto strLen = input.length();
    int len = MultiByteToWideChar(CP_UTF8, 0, cStr, strLen, NULL, 0);
    wchar_t* m_wchar = new wchar_t[len + 1];
    MultiByteToWideChar(CP_UTF8, 0, cStr, strLen, m_wchar, len);
    m_wchar[len] = L'\0';
    return m_wchar;
}

void AUtf8ToGbk(std::string& str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    str = szGBK;
    if (wszGBK) {
        delete[] wszGBK;
    }
    if (szGBK) {
        delete[] szGBK;
    }
}

__AStateHookManager::HookContainer& __AStateHookManager::GetHookContainer()
{
    static HookContainer hookContainer;
    return hookContainer;
}

void __AStateHookManager::RunBeforeScript()
{
    for (auto&& stateHook : GetHookContainer()) {
        stateHook->BeforeScript();
    }
}
void __AStateHookManager::RunAfterScript()
{
    for (auto&& stateHook : GetHookContainer()) {
        stateHook->AfterScript();
    }
}
void __AStateHookManager::RunEnterFight()
{
    for (auto&& stateHook : GetHookContainer()) {
        stateHook->EnterFight();
    }
}
void __AStateHookManager::RunExitFight()
{
    for (auto&& stateHook : GetHookContainer()) {
        stateHook->ExitFight();
    }
}
