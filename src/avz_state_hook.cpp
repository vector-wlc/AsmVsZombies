#include "avz_state_hook.h"

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
    for (auto&& stateHook : GetHookContainer()) {
        stateHook.second->Init();
    }
}

__AStateHookManager::HookContainer& __AStateHookManager::GetHookContainer()
{
    static HookContainer hookContainer;
    return hookContainer;
}

void __AStateHookManager::RunAllBeforeScript()
{
    if (_isRunBeforeScript) {
        return;
    }
    _isRunBeforeScript = true;
    for (auto&& stateHook : GetHookContainer()) {
        stateHook.second->RunBeforeScript();
    }
    if (__aInternalGlobal.pvzBase->GameUi() == 3) { // 如果直接进战斗界面需要立即调用 RunEnterFight
        RunAllEnterFight();
    }
}

void __AStateHookManager::RunAllAfterScript()
{
    if (_isRunAfterScript) {
        return;
    }
    _isRunAfterScript = true;
    for (auto&& stateHook : GetHookContainer()) {
        stateHook.second->RunAfterScript();
    }
}

void __AStateHookManager::RunAllEnterFight()
{
    if (_isRunEnterFight) {
        return;
    }
    _isRunEnterFight = true;
    for (auto&& stateHook : GetHookContainer()) {
        stateHook.second->RunEnterFight();
    }
}

void __AStateHookManager::RunAllExitFight()
{
    if (_isRunExitFight) {
        return;
    }
    _isRunExitFight = true;
    for (auto&& stateHook : GetHookContainer()) {
        stateHook.second->RunExitFight();
    }
}

void __APublicStateHook::Init()
{
    _isRunBeforeScript = false;
    _isRunAfterScript = false;
    _isRunEnterFight = false;
    _isRunExitFight = false;
}

void __APublicStateHook::RunBeforeScript()
{
    if (!_isRunBeforeScript) {
        _isRunBeforeScript = true;
        _BeforeScript();
    }
}

void __APublicStateHook::RunAfterScript()
{
    if (!_isRunAfterScript) {
        _isRunAfterScript = true;
        _AfterScript();
    }
}

void __APublicStateHook::RunEnterFight()
{
    if (!_isRunEnterFight) {
        _isRunEnterFight = true;
        _EnterFight();
    }
}

void __APublicStateHook::RunExitFight()
{
    if (!_isRunExitFight) {
        _isRunExitFight = true;
        _ExitFight();
    }
}
