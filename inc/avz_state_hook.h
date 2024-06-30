#ifndef __AVZ_STATE_HOOK_H__
#define __AVZ_STATE_HOOK_H__

#include "avz_global.h"

#define __ADefineHookClass(HookName)                                                      \
    class __APublic##HookName##Hook {                                                     \
    public:                                                                               \
        using HookContainer = std::multimap<int, __APublic##HookName##Hook*>;             \
        __APublic##HookName##Hook(int runOrder);                                          \
        virtual ~__APublic##HookName##Hook();                                             \
        void Run##HookName();                                                             \
        static void Reset();                                                              \
        static void RunAll();                                                             \
                                                                                          \
    protected:                                                                            \
        static HookContainer& _GetHookContainer();                                        \
        virtual void _##HookName() {};                                                    \
        HookContainer::iterator _iter;                                                    \
        bool _isRun = false;                                                              \
        static bool _isRunAll;                                                            \
    };                                                                                    \
    template <int hookOrder>                                                              \
    class AOrdered##HookName##Hook : protected __APublic##HookName##Hook {                \
    public:                                                                               \
        AOrdered##HookName##Hook()                                                        \
            : __APublic##HookName##Hook(hookOrder) {                                      \
        }                                                                                 \
        static constexpr int HOOK_ORDER = hookOrder;                                      \
    };                                                                                    \
    using A##HookName##Hook = AOrdered##HookName##Hook<0>;                                \
    template <int hookOrder>                                                              \
    __APublic##HookName##Hook& AToPublicHook(AOrdered##HookName##Hook<hookOrder>& hook) { \
        return *((__APublic##HookName##Hook*)(&hook));                                    \
    }

// 此函数会在 本框架 基本内存信息初始化完成后且调用 void AScript() 之前运行
__ADefineHookClass(BeforeScript);

// 此函数会在 本框架 调用 void AScript() 之后运行
__ADefineHookClass(AfterScript);

// 此函数会在游戏进入战斗界面后立即运行
__ADefineHookClass(EnterFight);

// 此函数会在游戏退出战斗界面后立即运行
// 特别注意: 如果用户从主界面进入选卡界面但是又立即退回主界面，此函数依然会运行
__ADefineHookClass(ExitFight);

// 此函数会在每次注入之后运行
// 注意此函数非常危险，此函数内无法使用很多 AvZ 的功能，至于无法使用哪些，
// 用户可以自行踩雷，因为实在是太多了，不想一一枚举
// 因为 AvZ 的初始化发生在进入战斗界面或者选卡界面的时候
__ADefineHookClass(AfterInject);

// 此函数会在每帧运行 AvZ 主体代码之前运行
// 注意此函数非常危险，最好判断一下当前 PvZ 的状态再使用 AvZ 的内置函数
__ADefineHookClass(BeforeTick);

// 此函数会在每帧运行 AvZ 主体代码之后运行
// 注意此函数非常危险，最好判断一下当前 PvZ 的状态再使用 AvZ 的内置函数
__ADefineHookClass(AfterTick);

// 此函数在 AvZ 因异常退出或者卸载前运行
__ADefineHookClass(BeforeExit);

template <typename... Types>
class __APublicStateHookT : public Types... {
public:
    __APublicStateHookT(int hookOrder)
        : Types(hookOrder)... {
    }
};

using __APublicStateHook = __APublicStateHookT<
    __APublicBeforeScriptHook,
    __APublicAfterScriptHook,
    __APublicEnterFightHook,
    __APublicExitFightHook,
    __APublicBeforeTickHook,
    __APublicAfterTickHook,
    __APublicAfterInjectHook,
    __APublicBeforeExitHook>;

template <int hookOrder>
class AOrderedStateHook : protected __APublicStateHook {
public:
    AOrderedStateHook()
        : __APublicStateHook(hookOrder) {
    }
    static constexpr int HOOK_ORDER = hookOrder;
};

template <int hookOrder>
__APublicStateHook& AToPublicHook(AOrderedStateHook<hookOrder>& hook) {
    return *((__APublicStateHook*)(&hook));
}

using AStateHook = AOrderedStateHook<0>;

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
