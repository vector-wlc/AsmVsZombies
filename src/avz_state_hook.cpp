#include "libavz.h"

#define __ADefineHookClassFuncs(HookName)                                                    \
    __APublic##HookName##Hook::__APublic##HookName##Hook(int runOrder)                       \
    {                                                                                        \
        __ARegisterInitOp([this] {                                                           \
            this->_isRunAll = false;                                                         \
            this->_isRun = false;                                                            \
        });                                                                                  \
        _iter = _GetHookContainer().emplace(runOrder, this);                                 \
    }                                                                                        \
    __APublic##HookName##Hook::~__APublic##HookName##Hook()                                  \
    {                                                                                        \
        _GetHookContainer().erase(_iter);                                                    \
    }                                                                                        \
    void __APublic##HookName##Hook::Run##HookName()                                          \
    {                                                                                        \
        if (!_isRun) {                                                                       \
            _##HookName();                                                                   \
            _isRun = true;                                                                   \
        }                                                                                    \
    }                                                                                        \
    void __APublic##HookName##Hook::Reset()                                                  \
    {                                                                                        \
        _isRunAll = false;                                                                   \
        for (auto&& hook : _GetHookContainer()) {                                            \
            hook.second->_isRun = false;                                                     \
        }                                                                                    \
    }                                                                                        \
    void __APublic##HookName##Hook::RunAll()                                                 \
    {                                                                                        \
        if (!_isRunAll) {                                                                    \
            for (auto&& hook : _GetHookContainer()) {                                        \
                hook.second->Run##HookName();                                                \
            }                                                                                \
            _isRunAll = true;                                                                \
        }                                                                                    \
    }                                                                                        \
    __APublic##HookName##Hook::HookContainer& __APublic##HookName##Hook::_GetHookContainer() \
    {                                                                                        \
        static HookContainer _;                                                              \
        return _;                                                                            \
    }                                                                                        \
    bool __APublic##HookName##Hook::_isRunAll = false

__ADefineHookClassFuncs(BeforeScript);
__ADefineHookClassFuncs(AfterScript);
__ADefineHookClassFuncs(EnterFight);
__ADefineHookClassFuncs(ExitFight);
__ADefineHookClassFuncs(AfterInject);
__ADefineHookClassFuncs(BeforeTick);
__ADefineHookClassFuncs(AfterTick);
__ADefineHookClassFuncs(BeforeExit);
