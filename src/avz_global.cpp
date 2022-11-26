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

bool __AStateHookManager::_isRunBeforeScript = false;
bool __AStateHookManager::_isRunAfterScript = false;
bool __AStateHookManager::_isRunEnterFight = false;
bool __AStateHookManager::_isRunExitFight = false;

void __AStateHookManager::Init()
{
    _isRunBeforeScript = false;
    _isRunAfterScript = false;
    _isRunEnterFight = false;
    _isRunExitFight = false;
}

__AStateHookManager::HookContainer& __AStateHookManager::GetHookContainer()
{
    static HookContainer hookContainer;
    return hookContainer;
}

void __AStateHookManager::RunBeforeScript()
{
    if (_isRunBeforeScript) {
        return;
    }
    _isRunBeforeScript = true;

    for (auto&& stateHook : GetHookContainer()) {
        stateHook->BeforeScript();
    }
    if (__aInternalGlobal.pvzBase->GameUi() == 3) { // 如果直接进战斗界面需要立即调用 RunEnterFight
        RunEnterFight();
    }
}

void __AStateHookManager::RunAfterScript()
{
    if (_isRunAfterScript) {
        return;
    }
    _isRunAfterScript = true;
    for (auto&& stateHook : GetHookContainer()) {
        stateHook->AfterScript();
    }
}

void __AStateHookManager::RunEnterFight()
{
    if (_isRunEnterFight) {
        return;
    }
    _isRunEnterFight = true;
    for (auto&& stateHook : GetHookContainer()) {
        stateHook->EnterFight();
    }
}

void __AStateHookManager::RunExitFight()
{
    if (_isRunExitFight) {
        return;
    }
    _isRunExitFight = true;
    for (auto&& stateHook : GetHookContainer()) {
        stateHook->ExitFight();
    }
}
