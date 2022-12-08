/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-12-06 12:56:14
 * @Description:
 */
#ifndef __AVZ_STATE_HOOK_H__
#define __AVZ_STATE_HOOK_H__

#include "avz_global.h"

class __APublicStateHook;
class __AStateHookManager {
public:
    using HookContainer = std::multimap<int, __APublicStateHook*>;

    static void RunAllBeforeScript();
    static void RunAllAfterScript();
    static void RunAllEnterFight();
    static void RunAllExitFight();
    static void Init();
    __ANodiscard static HookContainer& GetHookContainer();

protected:
    static bool _isRunBeforeScript;
    static bool _isRunAfterScript;
    static bool _isRunEnterFight;
    static bool _isRunExitFight;
};

class __APublicStateHook {
public:
    __APublicStateHook(int runOrder)
    {
        _iter = __AStateHookManager::GetHookContainer().emplace(runOrder, this);
    }

    void Init();
    void RunBeforeScript();
    void RunAfterScript();
    void RunEnterFight();
    void RunExitFight();

    virtual ~__APublicStateHook()
    {
        __AStateHookManager::GetHookContainer().erase(_iter);
    }

protected:
    __AStateHookManager::HookContainer::iterator _iter;
    bool _isRunBeforeScript = false;
    bool _isRunAfterScript = false;
    bool _isRunEnterFight = false;
    bool _isRunExitFight = false;

    // 此函数会在 本框架 基本内存信息初始化完成后且调用 void Script() 之前运行
    virtual void _BeforeScript() { }

    // 此函数会在 本框架 调用 void Script() 之后运行
    virtual void _AfterScript() { }

    // 此函数会在游戏进入战斗界面后立即运行
    virtual void _EnterFight() { }

    // 此函数会在游戏退出战斗界面后立即运行
    // 特别注意: 如果用户从主界面进入选卡界面但是又立即退回主界面，此函数依然会运行
    virtual void _ExitFight() { }
};

// hookOrder 默认为 0, 数值越小, AStateHook 越先运行
template <int hookOrder>
class AOrderedStateHook : protected __APublicStateHook {
public:
    AOrderedStateHook()
        : __APublicStateHook(hookOrder)
    {
    }
    static constexpr int HOOK_ORDER = hookOrder;
};

using AStateHook = AOrderedStateHook<0>;

template <int hookOrder>
__APublicStateHook& AToPublicHook(AOrderedStateHook<hookOrder>& hook)
{
    return *((__APublicStateHook*)(&hook));
}

template <typename FirstHook, typename... OthersHook>
    requires(FirstHook::HOOK_ORDER < INT_MAX)
class __AMaxHookT {
public:
    static constexpr int VALUE = std::max(FirstHook::HOOK_ORDER, __AMaxHookT<OthersHook...>::VALUE);
};

template <typename FirstHook>
    requires(FirstHook::HOOK_ORDER < INT_MAX)
class __AMaxHookT<FirstHook> {
public:
    static constexpr int VALUE = FirstHook::HOOK_ORDER;
};

template <typename... Hooks>
constexpr int AAfterHook = __AMaxHookT<Hooks...>::VALUE + 1;

template <typename FirstHook, typename... OthersHook>
    requires(FirstHook::HOOK_ORDER > INT_MIN)
class __AMinHookT {
public:
    static constexpr int VALUE = std::min(FirstHook::HOOK_ORDER, __AMaxHookT<OthersHook...>::VALUE);
};

template <typename FirstHook>
    requires(FirstHook::HOOK_ORDER > INT_MIN)
class __AMinHookT<FirstHook> {
public:
    static constexpr int VALUE = FirstHook::HOOK_ORDER;
};

template <typename... Hooks>
constexpr int ABeforeHook = __AMinHookT<Hooks...>::VALUE - 1;

#endif