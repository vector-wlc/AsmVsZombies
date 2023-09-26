#include "avz_state_hook.h"

bool __AStateHookManager::_isRunBeforeScript = false;
bool __AStateHookManager::_isRunAfterScript = false;
bool __AStateHookManager::_isRunEnterFight = false;
bool __AStateHookManager::_isRunExitFight = false;
bool __AStateHookManager::_isRunAfterInject = false;

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

#define GenerateStateHookManagerCode(FuncName)        \
    void __AStateHookManager::RunAll##FuncName()      \
    {                                                 \
        if (_isRun##FuncName) {                       \
            return;                                   \
        }                                             \
        _isRun##FuncName = true;                      \
        for (auto&& stateHook : GetHookContainer()) { \
            stateHook.second->Run##FuncName();        \
        }                                             \
    }

GenerateStateHookManagerCode(AfterScript);
GenerateStateHookManagerCode(EnterFight);
GenerateStateHookManagerCode(ExitFight);
GenerateStateHookManagerCode(AfterInject);

void __APublicStateHook::Init()
{
    _isRunBeforeScript = false;
    _isRunAfterScript = false;
    _isRunEnterFight = false;
    _isRunExitFight = false;
}

#define GenerateStateHookCode(FuncName)      \
    void __APublicStateHook::Run##FuncName() \
    {                                        \
        if (!_isRun##FuncName) {             \
            _isRun##FuncName = true;         \
            _##FuncName();                   \
        }                                    \
    }

GenerateStateHookCode(BeforeScript);
GenerateStateHookCode(AfterScript);
GenerateStateHookCode(EnterFight);
GenerateStateHookCode(ExitFight);
GenerateStateHookCode(AfterInject);
